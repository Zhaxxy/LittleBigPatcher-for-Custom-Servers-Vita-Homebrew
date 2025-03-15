/*
this was modified from that with some help looking at vitashell

https://github.com/vitasdk/samples/blob/a799c2a8139915bd51706c198c493054dce30dac/ime/src/main.c
*/
#include <string.h>
#include <stdbool.h>

#include <psp2/types.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/message_dialog.h>
#include <psp2/ime_dialog.h>
#include <psp2/display.h>
#include <psp2/apputil.h>
#include <psp2/gxm.h>
#include <psp2/kernel/sysmem.h>

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))
#define DISPLAY_WIDTH			960
#define DISPLAY_HEIGHT			544
#define DISPLAY_STRIDE_IN_PIXELS	1024
#define DISPLAY_BUFFER_COUNT		2
#define DISPLAY_MAX_PENDING_SWAPS	1

// taken from VitaShell
// https://github.com/TheOfficialFloW/VitaShell/blob/81af70971ba18b8ce86215b04180f1e3d21cdfc9/ime_dialog.c#L32-L72

static void utf16_to_utf8(const uint16_t *src, uint8_t *dst) {
  int i;
  for (i = 0; src[i]; i++) {
    if ((src[i] & 0xFF80) == 0) {
      *(dst++) = src[i] & 0xFF;
    } else if((src[i] & 0xF800) == 0) {
      *(dst++) = ((src[i] >> 6) & 0xFF) | 0xC0;
      *(dst++) = (src[i] & 0x3F) | 0x80;
    } else if((src[i] & 0xFC00) == 0xD800 && (src[i + 1] & 0xFC00) == 0xDC00) {
      *(dst++) = (((src[i] + 64) >> 8) & 0x3) | 0xF0;
      *(dst++) = (((src[i] >> 2) + 16) & 0x3F) | 0x80;
      *(dst++) = ((src[i] >> 4) & 0x30) | 0x80 | ((src[i + 1] << 2) & 0xF);
      *(dst++) = (src[i + 1] & 0x3F) | 0x80;
      i += 1;
    } else {
      *(dst++) = ((src[i] >> 12) & 0xF) | 0xE0;
      *(dst++) = ((src[i] >> 6) & 0x3F) | 0x80;
      *(dst++) = (src[i] & 0x3F) | 0x80;
    }
  }

  *dst = '\0';
}

static void utf8_to_utf16(const uint8_t *src, uint16_t *dst) {
  int i;
  for (i = 0; src[i];) {
    if ((src[i] & 0xE0) == 0xE0) {
      *(dst++) = ((src[i] & 0x0F) << 12) | ((src[i + 1] & 0x3F) << 6) | (src[i + 2] & 0x3F);
      i += 3;
    } else if ((src[i] & 0xC0) == 0xC0) {
      *(dst++) = ((src[i] & 0x1F) << 6) | (src[i + 1] & 0x3F);
      i += 2;
    } else {
      *(dst++) = src[i];
      i += 1;
    }
  }

  *dst = '\0';
}


typedef struct{
	void*data;
	SceGxmSyncObject*sync;
	SceGxmColorSurface surf;
	SceUID uid;
}displayBuffer;

unsigned int backBufferIndex = 0;
unsigned int frontBufferIndex = 0;
/* could be converted as struct displayBuffer[] */
displayBuffer dbuf[DISPLAY_BUFFER_COUNT];

void *dram_alloc(unsigned int size, SceUID *uid){
	void *mem;
	*uid = sceKernelAllocMemBlock("gpu_mem", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, ALIGN(size,256*1024), NULL);
	sceKernelGetMemBlockBase(*uid, &mem);
	sceGxmMapMemory(mem, ALIGN(size,256*1024), SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE);
	return mem;
}
void gxm_vsync_cb(const void *callback_data){
	sceDisplaySetFrameBuf(&(SceDisplayFrameBuf){sizeof(SceDisplayFrameBuf),
		*((void **)callback_data),DISPLAY_STRIDE_IN_PIXELS, 0,
		DISPLAY_WIDTH,DISPLAY_HEIGHT}, SCE_DISPLAY_SETBUF_NEXTFRAME);
}
void gxm_init(){
	sceGxmInitialize(&(SceGxmInitializeParams){0,DISPLAY_MAX_PENDING_SWAPS,gxm_vsync_cb,sizeof(void *),SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE});
	unsigned int i;
	for (i = 0; i < DISPLAY_BUFFER_COUNT; i++) {
		dbuf[i].data = dram_alloc(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, &dbuf[i].uid);
		sceGxmColorSurfaceInit(&dbuf[i].surf,SCE_GXM_COLOR_FORMAT_A8B8G8R8,SCE_GXM_COLOR_SURFACE_LINEAR,SCE_GXM_COLOR_SURFACE_SCALE_NONE,SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS,dbuf[i].data);
		sceGxmSyncObjectCreate(&dbuf[i].sync);
	}
}
void gxm_swap(){
	sceGxmPadHeartbeat(&dbuf[backBufferIndex].surf, dbuf[backBufferIndex].sync);
	sceGxmDisplayQueueAddEntry(dbuf[frontBufferIndex].sync, dbuf[backBufferIndex].sync, &dbuf[backBufferIndex].data);
	frontBufferIndex = backBufferIndex;
	backBufferIndex = (backBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
}
void gxm_term(){
	sceGxmTerminate();

	for (int i=0; i<DISPLAY_BUFFER_COUNT; ++i)
		sceKernelFreeMemBlock(dbuf[i].uid);
}

void init_for_input () {
	sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
	sceCommonDialogSetConfigParam(&(SceCommonDialogConfigParam){});
	
	
}

int input(const char* title, char* text, uint32_t size) {
	
	int input2_size = (size*2) + 1;
	uint16_t input2[input2_size];
	memset(input2, 0, input2_size);
	
	int title_utf16_size = (strlen(title)*2) + 1;
	uint16_t title_utf16[title_utf16_size];
	memset(title_utf16, 0, title_utf16_size);
	
	utf8_to_utf16(title,title_utf16);
	utf8_to_utf16(text,input2);
	
	
	SceImeDialogParam param;
	int shown_dial = 0;
	SceImeDialogResult result={};
	bool finshed_the_dialog = 0;
	while (!finshed_the_dialog) {
		vita2d_start_drawing();
		vita2d_clear_screen();

		//clear current screen buffer
		//memset(dbuf[backBufferIndex].data,0xff000000,DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4);

		if (!shown_dial) {
			sceImeDialogParamInit(&param);

			param.supportedLanguages = SCE_IME_LANGUAGE_ENGLISH;
			param.languagesForced = SCE_TRUE;
			param.type = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
			param.option = 0;
			param.textBoxMode = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
			param.title = title_utf16;
			param.maxTextLength = size-1;
			param.initialText = input2;
			param.inputTextBuffer = input2;

			shown_dial = sceImeDialogInit(&param) > 0;
		}

		if (sceImeDialogGetStatus() == SCE_COMMON_DIALOG_STATUS_FINISHED) {
			memset(&result, 0, sizeof(result));
			sceImeDialogGetResult(&result);
			finshed_the_dialog = 1;
			sceImeDialogTerm();
			shown_dial = 0;/*< to respawn sceImeDialogInit on next loop */
		}

		sceCommonDialogUpdate(&(SceCommonDialogUpdateParam){{
			NULL,dbuf[backBufferIndex].data,0,0,
			DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS},
			dbuf[backBufferIndex].sync});


        vita2d_end_drawing();
		vita2d_common_dialog_update();
        vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	vita2d_start_drawing();
	vita2d_clear_screen();
	if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
		utf16_to_utf8(input2,text);
	}
	return result.button;
}