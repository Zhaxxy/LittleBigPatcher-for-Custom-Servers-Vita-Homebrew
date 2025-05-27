#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include <string.h>
#include <psp2/ctrl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <psp2/power.h> // for scePowerRequestColdReset
#include <psp2/shellutil.h>  // for sceShellUtilLock
#include <psp2/apputil.h>  // for sceAppUtilReceiveAppEvent and SceAppUtilAppEventParam
#include <psp2/system_param.h> // for sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON

#include <psp2/kernel/clib.h> // for sceClibPrintf

#include <assert.h>

#include <stdarg.h>
#include <vita2d.h>

#include "types_for_my_patcher_app.h"

#include "NotoSans_Regular_ttf.h"
#include "text_input_vita.h"
#include "save_folders.h"
#include "colours_config.h"
#include "patching_eboot_elf_vita_dlc_lock_removal.h"
#include "for_elfinject_globals.h"
#include "read_sfo.h"
#include "copyfile_thing.h"
#include "vita-unmake-fself/vita_unmake_fself.h"
#include "elf_injector/elf_inject.h"
#include "sha1.h" // for checking if eboot.bin is known

#include "lua-5.4.7/src/lua.h"
#include "lua-5.4.7/src/lauxlib.h"
#include "lua-5.4.7/src/lualib.h"

#define MAX_URL_LEN_INCL_NULL 72
#define MAX_DIGEST_LEN_INCL_NULL 20

#include <time.h>

#define BTN_LEFT       SCE_CTRL_LEFT
#define BTN_DOWN       SCE_CTRL_DOWN
#define BTN_RIGHT      SCE_CTRL_RIGHT
#define BTN_UP         SCE_CTRL_UP
#define BTN_START      SCE_CTRL_START
#define BTN_R3         SCE_CTRL_R3
#define BTN_L3         SCE_CTRL_L3
#define BTN_SELECT     SCE_CTRL_SELECT  
#define BTN_SQUARE     SCE_CTRL_SQUARE

// globals instead of macros because we need to swap them in init if the user has circle for enter
int BTN_CROSS;
int BTN_CIRCLE;

#define BTN_TRIANGLE   SCE_CTRL_TRIANGLE
#define BTN_R1         SCE_CTRL_R1
#define BTN_L1         SCE_CTRL_L1
#define BTN_R2         SCE_CTRL_R2
#define BTN_L2         SCE_CTRL_L2

#define MAX_LINE_LEN_OF_URL_ENTRY MAX_URL_LEN_INCL_NULL - 1 + MAX_DIGEST_LEN_INCL_NULL - 1 + sizeof(" ")

#define SECOND_THREAD_NAME "second_thread"
#define SECOND_THREAD_PRIORITY 0x10000100 // 0 means highest, value from sample
#define SECOND_THREAD_STACK_SIZE 0x500000 // 5mb

#define THREAD_RET_EBOOT_REVERTED 3
#define THREAD_RET_EBOOT_BAK_NO_EXIST 4
#define THREAD_RET_EBOOT_DECRYPT_FAILED 1
#define THREAD_RET_EBOOT_PATCH_FAILED 5
#define THREAD_RET_EBOOT_PATCHED 6
#define THREAD_RET_EBOOT_BACKUP_FAILED 7
#define THREAD_RET_REPATCH_INSTALLED 8
#define THREAD_RET_REPATCH_INSTALL_FAILED 9
#define THREAD_RET_UNKNOWN_EBOOT 10

#define THREAD_CURRENT_STATE_CLEANING_WORKSPACE 1
#define THREAD_CURRENT_STATE_COPYING_FROM_FAGDEC 2
#define THREAD_CURRENT_STATE_EBOOT_DECRYPT 3
#define THREAD_CURRENT_STATE_CALCING_EBOOT_ELF_SHA1 4
#define THREAD_CURRENT_STATE_PATCHING_VITA_DLC_UNLOCK 5
#define THREAD_CURRENT_STATE_START_PATCHING 6
#define THREAD_CURRENT_STATE_DONE_PATCHING 8
#define THREAD_CURRENT_STATE_EBOOT_ENCRYPT 9
#define THREAD_CURRENT_STATE_MAKE_REPATCH_FOLDERS 10
#define THREAD_CURRENT_STATE_FINAL_COPY_EBOOT_TO_REPATCH 11

#define ERROR_YET_TO_PRESS_OK_SUCCESS 2
#define ERROR_YET_TO_PRESS_OK_FAIL 1

#define MENU_MAIN 0
#define MENU_MAIN_ARROW 5-1

#define MENU_SELECT_URLS 1
#define MENU_SELECT_URLS_ARROW saved_urls_count-1

#define MENU_EDIT_URLS 2
#define MENU_EDIT_URLS_ARROW (saved_urls_count-1)*2+1


#define MENU_PATCH_GAMES_ARROW_NOT_INCL_PATCHES 6-1
#define MINUS_MENU_ARROW_AMNT_TO_GET_PATCH_LUA_INDEX MENU_PATCH_GAMES_ARROW_NOT_INCL_PATCHES + 1
#define MENU_PATCH_GAMES_ARROW MENU_PATCH_GAMES_ARROW_NOT_INCL_PATCHES+method_count
#define MENU_PATCH_GAMES 3

#define MENU_BROWSE_GAMES 4


#define YES_NO_POPUP_ARROW 2-1 // YES OR NO
#define YES_NO_GAME_POPUP_REVERT_EBOOT 1
#define YES_NO_GAME_POPUP_PATCH_GAME 2
#define YES_NO_GAME_POPUP_INSTALL_REPATCH 3
#define YES_NO_GAME_POPUP_REMOVE_ALLEFRESHER 4

#define TITLE_ID_PATCH 1
#define TITLE_ID_APP 2

#define REPATCH_INSTALLED_REPATCH 1
#define REPATCH_INSTALLED_ALLEFRESHER 2

// globals instead of macros because we need to swap them in init if the user has circle for enter
char MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CROSS_BTN[3];
char MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CIRCLE_BTN[3];

#define MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_SQUARE_BTN ""
#define MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_TRIANGLE_BTN ""


#define START_X_FOR_PRESS_TO_REFRESH_THINGS_TEXT 222

#define DEFAULT_TITLE_ID "PCSF00000"

#define DONE_A_SWITCH has_done_a_switch = 1; if (load_global_title_id()) {global_title_id_folder_type = 0;} 0; load_user_join_pwd(second_thread_args.join_password)

#define DRAW_CHAR_BG_COLOUR_HEIGHT 19
#define CHARACTER_HEIGHT 23
#define TEXT_SIZE 22

#define MAX_LINES 23-2 // minus 2 for title, text is alot wider and less taller on vita

#define MAX_CAPITIAL_W_CHARACTERS_PER_LINE 30
#define NEW_LINES_AMNT_PER_DIGIT_OF_X_INCREASE 6 // seems to be good

#define LIVEAREA_SELECTED(target_str,event_param) memcmp(event_param.dat,target_str,strlen(target_str)) == 0

lua_State *L;

int global_current_x;

struct TitleIdAndGameName {
	int title_id_folder_type;
	char title_id[sizeof(DEFAULT_TITLE_ID)];
	char game_name[128];
};

struct LuaPatchDetails {
	char patch_name[PATCH_LUA_SIZE];
	char patch_method[PATCH_METHOD_LUA_STRING_SIZE];
};

struct UrlToPatchTo {
	char url[MAX_URL_LEN_INCL_NULL];
	char digest[MAX_DIGEST_LEN_INCL_NULL];
};


struct UrlToPatchTo saved_urls[MAX_LINES-1];
#define RESET_SELECTED_URL_INDEX sizeof(saved_urls) / sizeof(saved_urls[0]) + 1
s8 selected_url_index = RESET_SELECTED_URL_INDEX;
s8 saved_urls_count = 0;
char global_title_id[sizeof(DEFAULT_TITLE_ID)] = DEFAULT_TITLE_ID;
int global_title_id_folder_type = 0;

int get_button_pressed(SceCtrlData *pad) {
	int result;
	sceCtrlPeekBufferPositive(0, pad, 1);
	result = pad->buttons;

	// accept left analog stick inputs as dpad inputs
	if (pad->ly < 64) {
		result |= BTN_UP;
	}
	else if (pad->ly > 192) {
		result |= BTN_DOWN;
	}
	if (pad->lx < 64) {
		result |= BTN_LEFT;
	}
	else if (pad->lx > 192) {
		result |= BTN_RIGHT;
	}
	return result;
}

int set_arrow(int menu_arrow,int btn_pressed, int max_arrow) 
{
	int new_arrow = menu_arrow;

	if (btn_pressed & BTN_UP) {
		new_arrow--;
	}
	else if (btn_pressed & BTN_DOWN) {
		new_arrow++;
	}

	if (new_arrow < 0) {
		new_arrow = max_arrow;
	}
	else if (new_arrow > max_arrow) {
		new_arrow = 0;
	}
	return new_arrow;
}

bool is_valid_title_id(char* title_id) // assumes its uppercase
{
	if(strlen(title_id) != 9) {
		return 0;
	}
	for (int i = 0; i < 4; i++) {
		if(!isalpha(title_id[i])) {
			return 0;
		}
	}
	for (int i = 5; i < 9; i++) {
		if(!(title_id[i] >= '0' && title_id[i] <= '9')) {
			return 0;
		}
	}
	
	return 1;
}


int save_user_join_pwd(const char * pretty_user_input_join_password) {
	FILE *fp = fopen(JOIN_PASSWORD_TXT, "wb");
	if (fp == 0) {
		return -1;
	}
	
	if (pretty_user_input_join_password[0] == 0) {
		fclose(fp);
		return 0;
	}
	
	fputs(pretty_user_input_join_password, fp);
	fclose(fp);
	return 0;
}

void load_user_join_pwd(char * pretty_user_input_join_password) {
	memset(pretty_user_input_join_password,0,4096+1);
	FILE *fp = fopen(JOIN_PASSWORD_TXT, "rb");
	if (fp == 0) {
		return;
	}
	
	fread(pretty_user_input_join_password,1,4096,fp);
	
	fclose(fp);
}

int save_global_title_id_to_disk() {
	FILE *fp = fopen(TITLE_ID_TXT, "wb");
	if (fp == 0) {
		return -1;
	}
	
	fwrite(global_title_id,1,sizeof(global_title_id)-1,fp);
	fclose(fp);
	return 0;
}
/*
returns 1 if the title id changed
*/
bool load_global_title_id() {
	char temp_title_id[sizeof(DEFAULT_TITLE_ID)] = {0};
	bool result;
	FILE *fp = fopen(TITLE_ID_TXT, "rb");
	if (fp == 0) {
		goto fail_to_load_title_id;
	}
	fseek(fp, 0, SEEK_END);
	if (ftell(fp) > 9) {
		char trailing_char[1];
		fseek(fp, 9, SEEK_SET);
		fread(trailing_char,1,sizeof(trailing_char),fp);
		if (!(isspace(trailing_char[0]))) {
			fclose(fp);
			goto fail_to_load_title_id;
		}
	}
	rewind(fp);
	
	fread(temp_title_id,1,sizeof(temp_title_id)-1,fp);
	
	for (int i = 0; temp_title_id[i] != '\0'; i++) {
		temp_title_id[i] = toupper(temp_title_id[i]);
	}
	
	if (!is_valid_title_id(temp_title_id)) {
		fclose(fp);
		goto fail_to_load_title_id;
	}
	
	fclose(fp);
	result = strcmp(temp_title_id,global_title_id) != 0;
	strcpy(global_title_id,temp_title_id);
	return result;
	
	fail_to_load_title_id:
	strcpy(global_title_id,DEFAULT_TITLE_ID);
	save_global_title_id_to_disk();
	return 1;
}

bool is_a_url_selected() {
	if (selected_url_index < 0 ) {
		return 0;
	}
	if (selected_url_index > saved_urls_count-1 ) {
		return 0;
	}
	
	return 1;
}

int title_id_exists(char * title_id)
{
	char fname[sizeof("ux0:/FAGDec/patch/ABCD12345/eboot.bin")];
	sprintf(fname,"ux0:/FAGDec/patch/%s/eboot.bin",title_id); // assumes that title_id is of lenght 9
	
	if (does_file_exist(fname)) {
		return TITLE_ID_PATCH;
	}
	
	sprintf(fname,"ux0:/FAGDec/app/%s/eboot.bin",title_id); // assumes that title_id is of lenght 9

	if (does_file_exist(fname)) {
		return TITLE_ID_APP;
	}
	return 0;
}

u32 total_count_of_patchable_games(u32 start_offset, u32 end_length)
{
	const char *fagdec_dirs[] = {"","ux0:/FAGDec/patch/", "ux0:/FAGDec/app/"};


	assert(end_length > 0);
	u32 total_count = 0;
	u32 start_counting = 0;
	DIR *game_dir;
	struct dirent* reader;
	
	for (int fagdec_dir_index = 1; fagdec_dir_index < 3; fagdec_dir_index++) {
		if (total_count >= end_length) {
			break;
		}
		game_dir = opendir(fagdec_dirs[fagdec_dir_index]);
		if (game_dir != NULL) {
			while ((reader = readdir(game_dir)) != NULL) {
				if (strcmp(reader->d_name,".") == 0 || strcmp(reader->d_name,"..") == 0) {
					continue;
				}
				if (!is_valid_title_id(reader->d_name)) {
					continue;
				}
				if (!title_id_exists(reader->d_name)) {
					continue;
				}
				
				if (start_counting >= start_offset) {
					total_count++;
					if (total_count >= end_length) {
						break;
					}
				}
				else {
					start_counting++;
				}

			}
			closedir(game_dir);
		}
	}
	return total_count;
}

u32 load_patchable_games(struct TitleIdAndGameName buffer[], u32 start_offset, u32 end_length)
{
	const char *fagdec_dirs[] = {"","ux0:/FAGDec/patch/", "ux0:/FAGDec/app/"};


	assert(end_length > 0);
	u32 total_count = 0;
	u32 start_counting = 0;
	DIR *game_dir;
	struct dirent* reader;
	char * game_name;
	char param_sfo_path[sizeof("ux0:/patch/ABCD12345/sce_sys/param.sfo")];
	
	for (int fagdec_dir_index = 1; fagdec_dir_index < 3; fagdec_dir_index++) {
		if (total_count >= end_length) {
			break;
		}
		game_dir = opendir(fagdec_dirs[fagdec_dir_index]);
		if (game_dir != NULL) {
			while ((reader = readdir(game_dir)) != NULL) {
				if (strcmp(reader->d_name,".") == 0 || strcmp(reader->d_name,"..") == 0) {
					continue;
				}
				if (!is_valid_title_id(reader->d_name)) {
					continue;
				}
				if (!title_id_exists(reader->d_name)) {
					continue;
				}
				
				if (start_counting >= start_offset) {
					buffer[total_count].title_id_folder_type = fagdec_dir_index;
					strcpy(buffer[total_count].title_id,reader->d_name);
					if (fagdec_dir_index == TITLE_ID_PATCH) {
						sprintf(param_sfo_path,"ux0:/patch/%s/sce_sys/param.sfo",reader->d_name); // ignore the warning on this line, we already ensured that the folder name is 9 chars long
					}
					else if (fagdec_dir_index == TITLE_ID_APP) {
						sprintf(param_sfo_path,"ux0:/app/%s/sce_sys/param.sfo",reader->d_name); // ignore the warning on this line, we already ensured that the folder name is 9 chars long
					}
					
					game_name = get_title_id_from_param(param_sfo_path);
					if (game_name == 0) {
						strcpy(buffer[total_count].game_name,"Unknown??");
					}
					else {
						strcpy(buffer[total_count].game_name,game_name);
						free(game_name);
					}

					total_count++;
					if (total_count >= end_length) {
						break;
					}
				}
				else {
					start_counting++;
				}

			}
			closedir(game_dir);
		}
	}
	return total_count;
}

/*
https://stackoverflow.com/questions/1488372/mimic-pythons-strip-function-in-c
*/
char *strstrip(char *s)
{
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}

void write_saved_urls(u8 saved_urls_txt_num) {
	struct UrlToPatchTo url_entry;
	char write_buffer[sizeof(url_entry.url) + 1 + sizeof(url_entry.digest) + 1 + 1];


	char filename[sizeof(SAVED_URLS_TXT_FIRST_HALF) + (sizeof("_ff")-1) + sizeof(SAVED_URLS_TXT_SECOND_HALF)];
	sprintf(filename,"%s_%d%s",SAVED_URLS_TXT_FIRST_HALF,saved_urls_txt_num,SAVED_URLS_TXT_SECOND_HALF);

	FILE *fp = fopen(filename, "wb+"); // not checking if it fails to open, just let it segfault, cause theres bigger problems if it doesnt works
	for (int i = 0; i < saved_urls_count; i++) {
		url_entry = saved_urls[i];
		if (url_entry.digest[0] != 0) {
			sprintf(write_buffer,"%s %s\n",url_entry.url,url_entry.digest);
		}
		else {
			sprintf(write_buffer,"%s\n",url_entry.url);
		}
		
		fprintf(fp,write_buffer);
	}
	fclose(fp);
	
}

void load_saved_urls(u8 saved_urls_txt_num) {
	u8 digest_offset_from_line;
	u8 digest_len;
    char * line = NULL;
	char * orig_line = NULL;
    size_t len = 0;
    ssize_t len_of_line;

	char filename[sizeof(SAVED_URLS_TXT_FIRST_HALF) + (sizeof("_ff")-1) + sizeof(SAVED_URLS_TXT_SECOND_HALF)];
	sprintf(filename,"%s_%d%s",SAVED_URLS_TXT_FIRST_HALF,saved_urls_txt_num,SAVED_URLS_TXT_SECOND_HALF);

	FILE *fp = fopen(filename, "ab+"); // not checking if it fails to open, just let it segfault, cause theres bigger problems if it doesnt works
    rewind(fp);
	int ready_url_i = 0;
	saved_urls_count = 0;
	while ((len_of_line = __getline(&orig_line, &len, fp)) > 0) {
		line = strstrip(orig_line);
		
		// remove any extra chars
		if (len_of_line > MAX_LINE_LEN_OF_URL_ENTRY) {
			line[MAX_LINE_LEN_OF_URL_ENTRY] = 0;
			len_of_line = MAX_LINE_LEN_OF_URL_ENTRY;
		}
		
		// getting all the characters after first space, not including the space
		digest_offset_from_line = strcspn(line, " ");
		digest_len = len_of_line - digest_offset_from_line;

		struct UrlToPatchTo temp_url;
		temp_url.url[0] = 0;
		temp_url.digest[0] = 0;		

		if (digest_len != 0) {
			digest_len--;
			// remove extra chars on digest
			if (digest_len > MAX_DIGEST_LEN_INCL_NULL-1)  {
				digest_len = MAX_DIGEST_LEN_INCL_NULL-1;
			}
			memcpy(temp_url.digest,line+digest_offset_from_line+1,digest_len);
			temp_url.digest[digest_len] = 0; // ensure it wont read leftover data
			
			// removing the digest off the line, itll just be left with the url
			line[digest_offset_from_line] = 0;
			len_of_line -= digest_len;
			len_of_line--; // for the space char
		}
		

		// remove any extra chars
		if(len_of_line > MAX_URL_LEN_INCL_NULL-1) {
			line[MAX_URL_LEN_INCL_NULL-1] = 0;
			len_of_line = MAX_URL_LEN_INCL_NULL;
		}
		
		if (len_of_line != 0) {
			strcpy(temp_url.url,line);
		}
		
		
		memcpy(&saved_urls[ready_url_i],&temp_url,sizeof(struct UrlToPatchTo));
		saved_urls_count++;
		
		
		ready_url_i++;
		if (ready_url_i >= sizeof(saved_urls) / sizeof(saved_urls[0])) {
			break;
		}
		
    }
	
	if (ready_url_i < sizeof(saved_urls) / sizeof(saved_urls[0])) {
		while (ready_url_i < sizeof(saved_urls) / sizeof(saved_urls[0])) {
			struct UrlToPatchTo temp_url_2;
			strcpy(temp_url_2.url,"ENTER_A_URL_HERE");
			strcpy(temp_url_2.digest,"");
			memcpy(&saved_urls[ready_url_i],&temp_url_2,sizeof(struct UrlToPatchTo));
			saved_urls_count++;
			ready_url_i++;
		}
	}
	

	
	fclose(fp);
	free(orig_line);

}

int save_patch_cache_bool(bool use_patch_cache) {
	if (use_patch_cache) {
		FILE *fp = fopen(PATCH_CACHE_FILE_EXISTS_THEN_TRUE, "wb");
		if (fp == 0) {
			return -1;
		}
		fprintf(fp,"1");
		fclose(fp);
		return 0;
	}
	else {
		DIR *cache_dir_fp = opendir(CACHE_DIR);
		struct dirent* reader;
		char full_path[1024];
		if (cache_dir_fp == NULL) {
			return -1;
		}
		while ((reader = readdir(cache_dir_fp)) != NULL) {
			if (strcmp(reader->d_name,".") == 0 || strcmp(reader->d_name,"..") == 0) {
				continue;
			}
			snprintf(full_path, sizeof(full_path), "%s%s", CACHE_DIR, reader->d_name);
			remove(full_path);
		}
		closedir(cache_dir_fp);
		return 0;
	}
}

#define CONFIG_MAX_LINE_LENGTH 256
/*
i try to go off this https://github.com/DaveeFTW/taihen-parser/
*/
int is_repatch_installed() {
	FILE *fp;
	char * orig_line = NULL;
    char * line = NULL;
    size_t len = 0;
    ssize_t len_of_line;
	int last_occurance;
	char * last_dot;
	bool in_kernel_section = 0;
	int skprx_extension_offset;
	int result = 0;
	fp = fopen("ux0:/tai/config.txt","rb");
	if (fp == 0) {
		fp = fopen("ur0:/tai/config.txt","rb");
		if (fp == 0) {
			return -1;
		}
	}

	// i am not going to handle halt point (! chars) fuck that shit
	while ((len_of_line = __getline(&orig_line, &len, fp)) > 0) {
		// have to check this first per line as per the page
		if (len_of_line > CONFIG_MAX_LINE_LENGTH) {
			continue;
		}
		line = strstrip(orig_line);

		// ignore comments
		line[strcspn(line, "#")] = 0;
		len_of_line = strlen(line);
		if (!line[0]) {
			continue;
		}
		// now we can parse the line
		if (line[0] == '*') {
			if (strcmp(line,"*KERNEL") == 0) {
				in_kernel_section = 1;
			}
			else {
				in_kernel_section = 0;
			}
			continue;
		}
		// finally start parsing the actual entry

		// get the last part of the path
		last_occurance = strrchr(line,'/');
		if (last_occurance != 0) {
			line = last_occurance+1;
		}



		// if theres and underscore in the name, we will only check the text before it
		line[strcspn(line, "_")] = 0;
		len_of_line = strlen(line);
		if (!line[0]) {
			continue;
		}
		// Remove the file subfix extension
		last_dot = strrchr(line, '.');
		if (last_dot) {
			*last_dot = 0;
		}

		if (strncmp(line,"Allefresher",sizeof("Allefresher")) == 0) {
			fclose(fp);
			free(orig_line);
			return REPATCH_INSTALLED_ALLEFRESHER;
		}

		if (strncmp(line,"repatch",sizeof("repatch")) == 0 && in_kernel_section) {
			result = REPATCH_INSTALLED_REPATCH;
		}

	}
	fclose(fp);
	free(orig_line);
	return result;
}


bool install_repatch(bool remove_allefresher) {
	int rename_and_remove_rets;
	bool is_ur0_config = 0;
	FILE *fp;
	FILE *fp_temp;
	char * orig_line = NULL;
    char * line = NULL;
    size_t len = 0;
    ssize_t len_of_line;
	int kernel_header_line_num = -1;
	int last_occurance;
	char * last_dot;
	int allefresher_line_num = -1;
	int line_num = 0;
	
	fp = fopen("ux0:/tai/config.txt","rb");
	if (fp == 0) {
		fp = fopen("ur0:/tai/config.txt","rb");
		if (fp == 0) {
			return 0;
		}
		is_ur0_config = 1;
	}

	// i am not going to handle halt point (! chars) fuck that shit
	while ((len_of_line = __getline(&orig_line, &len, fp)) > 0) {
		// have to check this first per line as per the page
		if (len_of_line > CONFIG_MAX_LINE_LENGTH) {
			line_num++; continue;
		}
		line = strstrip(orig_line);

		// ignore comments
		line[strcspn(line, "#")] = 0;
		len_of_line = strlen(line);
		if (!line[0]) {
			line_num++; continue;
		}
		// now we can parse the line
		if (line[0] == '*') {
			if (strcmp(line,"*KERNEL") == 0) {
				kernel_header_line_num = line_num;
				if (remove_allefresher) {
					line_num++; continue;
				}
				else {
					break;
				}
			}
		}
		if (remove_allefresher) {
			// parsing line to check for allerfresher again
			last_occurance = strrchr(line,'/');
			if (last_occurance != 0) {
				line = last_occurance+1;
			}

			// TODO This code allows for lines like ur0:tai/repatch but whos doing that?
			// 

			// if theres and underscore in the name, we will only check the text before it
			line[strcspn(line, "_")] = 0;
			len_of_line = strlen(line);
			if (!line[0]) {
				line_num++; continue;
			}
			// Remove the file subfix extension
			last_dot = strrchr(line, '.');
			if (last_dot) {
				*last_dot = 0;
			}

			if (strncmp(line,"Allefresher",sizeof("Allefresher")) == 0) {
				allefresher_line_num = line_num;
				line_num++; continue;
			}
		}
		line_num++;
	}

	if (remove_allefresher && allefresher_line_num == -1) {
		fclose(fp);
		free(orig_line);
		return 0;
	}
	else if (kernel_header_line_num == -1) {
		fclose(fp);
		free(orig_line);
		return 0;
	}
	rewind(fp);
	
	line_num = 0;
	fp_temp = fopen(TEMP_TAI_CONFIG,"wb");
	if (fp_temp == 0) {
		fclose(fp);
		free(orig_line);
		return 0;
	}
	kernel_header_line_num++; // shortcut incremnt so i will insert a new line after this
	while ((len_of_line = __getline(&orig_line, &len, fp)) > 0) {
		if (line_num == kernel_header_line_num && !remove_allefresher) {
			if (is_ur0_config) {
				fwrite(REPATCH_SKPRX_UR0_ENTRY,1,sizeof(REPATCH_SKPRX_UR0_ENTRY)-1,fp_temp); // do not write the null term
			}
			else {
				fwrite(REPATCH_SKPRX_UX0_ENTRY,1,sizeof(REPATCH_SKPRX_UX0_ENTRY)-1,fp_temp); // do not write the null term
			}
			
		}
		if (line_num == allefresher_line_num) {
			// comment out the line
			fwrite("# ",1,sizeof("# ")-1,fp_temp);
		}
		fwrite(orig_line,1,len_of_line,fp_temp); // do not write the null term

		line_num++;
	}
	fclose(fp_temp);

	fclose(fp);
	free(orig_line);

	if (is_ur0_config) {
		rename_and_remove_rets = copy_file(REPATCH_SKPRX_UR0_PATH,BUILT_IN_REPATCH_SKPRX_PATH);
		if (rename_and_remove_rets < 0) {
			return 0;
		}
		rename_and_remove_rets = copy_file("ur0:/tai/config.txt",TEMP_TAI_CONFIG);
		if (rename_and_remove_rets < 0) {
			return 0;
		}
	}
	else {
		rename_and_remove_rets = copy_file(REPATCH_SKPRX_UX0_PATH,BUILT_IN_REPATCH_SKPRX_PATH);
		if (rename_and_remove_rets < 0) {
			return 0;
		}
		rename_and_remove_rets = copy_file("ux0:/tai/config.txt",TEMP_TAI_CONFIG);
		if (rename_and_remove_rets < 0) {
			return 0;
		}
	}
	rename_and_remove_rets = remove(TEMP_TAI_CONFIG);
	if (rename_and_remove_rets < 0) {
		return 0;
	}
	return 1;

}

int install_repatch_thread(unsigned int arglen, void **argp) {
	struct SecondThreadArgs *args = *argp;
	
	if (install_repatch(args->remove_allefresher)) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_REPATCH_INSTALLED);
		return THREAD_RET_REPATCH_INSTALLED;
	}
	else {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_REPATCH_INSTALL_FAILED);
		return THREAD_RET_REPATCH_INSTALL_FAILED;
	}
}

int revert_patches_thread(unsigned int arglen, void **argp) {
	struct SecondThreadArgs *args = *argp;
	char repatch_eboot_bin_path[sizeof("ux0:/rePatch/abcd12345/eboot.bin")];
	sprintf(repatch_eboot_bin_path,"ux0:/rePatch/%s/eboot.bin",args->title_id);

	if (remove(repatch_eboot_bin_path) == 0) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_REVERTED);
		return THREAD_RET_EBOOT_REVERTED;
	}
	else {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_BAK_NO_EXIST);
		return THREAD_RET_EBOOT_BAK_NO_EXIST;
	}
}

int apply_patches_thread(unsigned int arglen, void **argp) {
	int unmake_fself_res;
	int copy_file_res;
	int patch_res = 0;
	struct SecondThreadArgs *args = *argp;
	struct UrlToPatchTo my_url = saved_urls[selected_url_index];
	char fagdec_eboot[sizeof("ux0:/FAGDec/patch/ABCD12345/eboot.bin")];
	char repatch_eboot_bin_path[sizeof("ux0:/rePatch/abcd12345/eboot.bin")];
	char repatch_title_id_folder[sizeof("ux0:/rePatch/abcd12345/")];
	
	SHA1_CTX ctx;
	FILE *fp_for_eboot;
	int eboot_chunk_size;
	uint8_t sha1_digest[SHA1_BLOCK_SIZE-1];

	char lua_func_name[PATCH_LUA_SIZE + sizeof("patch_")];

	sprintf(repatch_eboot_bin_path,"ux0:/rePatch/%s/eboot.bin",args->title_id);
	sprintf(repatch_title_id_folder,"ux0:/rePatch/%s/",args->title_id);
	
	switch (args->title_id_folder_type) {
		case TITLE_ID_PATCH:
			sprintf(fagdec_eboot,"ux0:/FAGDec/patch/%s/eboot.bin",args->title_id);
			break;
		case TITLE_ID_APP:
			sprintf(fagdec_eboot,"ux0:/FAGDec/app/%s/eboot.bin",args->title_id);
			break;
		default:
			assert(0);
	}

	// ensure working from clean state
	args->current_state = THREAD_CURRENT_STATE_CLEANING_WORKSPACE;
	sceClibPrintf("Cleaning workspace\n");
	remove(WORKING_DIR "eboot.bin.elf");
	remove(WORKING_DIR "eboot.bin");
	
	if (does_file_exist(WORKING_DIR "eboot.bin.elf") || does_file_exist(WORKING_DIR "eboot.bin.bin")) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
		return THREAD_RET_EBOOT_BACKUP_FAILED;
	}
	
	// variables only use if use_patch_cache
	int line_num_of_caches = 0;
	char * current_cache_line;
	FILE *fp;
	//
	if (args->use_patch_cache) {
		int current_cache_line_len = strlen(my_url.url) + strlen(my_url.digest) + strlen(args->title_id) + strlen(args->patch_lua_name) + strlen("FFFFFFFF") + strlen(args->join_password);;
		// TODO memory leak here, but this can only happen once so it does not really matter
		current_cache_line = malloc(current_cache_line_len + 1);
		if (!current_cache_line) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
			return THREAD_RET_EBOOT_BACKUP_FAILED;
		}
		snprintf(current_cache_line,current_cache_line_len + 1,"%s%s%s%08X%s%s",my_url.url,my_url.digest,args->title_id,args->title_id_folder_type,args->patch_lua_name,args->join_password);
		sceClibPrintf("Checking if theres a cache for %s",current_cache_line);
		
		fp = fopen(CACHE_TXT_FILE, "ab+");
		if (fp == 0) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
			return THREAD_RET_EBOOT_BACKUP_FAILED;
		}
		rewind(fp);
		
		char * line = NULL;
		size_t len = 0;
		ssize_t len_of_line;
		
		while ((len_of_line = __getline(&line, &len, fp)) > 0) {
			if (len_of_line > 1000) {
				goto continue_and_incr_line_num;
			}
			line[strcspn(line, "\r\n")] = 0;
			len_of_line = strlen(line);
			if (!line[0]) {
				goto continue_and_incr_line_num;
			}
			if (len_of_line != current_cache_line_len) {
				goto continue_and_incr_line_num;
			}
			if (memcmp(current_cache_line,line,current_cache_line_len) == 0) {
				fclose(fp);
				args->current_state = THREAD_CURRENT_STATE_COPYING_FROM_FAGDEC;
				sceClibPrintf("Cache hit, restoring eboot from cache");
				char cache_eboot_bin_file[sizeof(CACHE_DIR "eboot_bin_repatch_vita_num_9999.bin")];
				sprintf(cache_eboot_bin_file,CACHE_DIR "eboot_bin_repatch_vita_num_%d.bin",line_num_of_caches);
				copy_file_res = copy_file(repatch_eboot_bin_path,cache_eboot_bin_file);
				if (copy_file_res == -1) {
					args->has_finished = 1;
					sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
					return THREAD_RET_EBOOT_BACKUP_FAILED;
				}
				args->has_finished = 1;
				sceKernelExitThread(THREAD_RET_EBOOT_PATCHED);
				return THREAD_RET_EBOOT_BACKUP_FAILED;
				// never will reach here
			}
			continue_and_incr_line_num:
			line_num_of_caches++;
		}
		fclose(fp);
	}
	
	args->current_state = THREAD_CURRENT_STATE_COPYING_FROM_FAGDEC;
	sceClibPrintf("Copying FAGDec eboot to workspace\n");
	copy_file_res = copy_file(WORKING_DIR "eboot.bin",fagdec_eboot);
	
	if (copy_file_res < 0) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
		return THREAD_RET_EBOOT_BACKUP_FAILED;
	}
	
	if (!does_file_exist(WORKING_DIR "eboot.bin")) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
		return THREAD_RET_EBOOT_BACKUP_FAILED;
	}
	args->current_state = THREAD_CURRENT_STATE_EBOOT_DECRYPT;
	sceClibPrintf("Decrypting/decompressing eboot.bin with vita-unmake-fself\n");
	unmake_fself_res = unmake_fself(WORKING_DIR "eboot.bin",WORKING_DIR "eboot.bin.elf");
	
	if (unmake_fself_res != 0) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_DECRYPT_FAILED);
		return THREAD_RET_EBOOT_DECRYPT_FAILED;
	}
	
	if (!does_file_exist(WORKING_DIR "eboot.bin.elf")) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_DECRYPT_FAILED);
		return THREAD_RET_EBOOT_DECRYPT_FAILED;
	}
	args->current_state = THREAD_CURRENT_STATE_CALCING_EBOOT_ELF_SHA1;
	sceClibPrintf("Calculating sha1 of eboot.bin.elf\n");
	if (args->offset_based_patch != 0) {
		sha1_init(&ctx);
		fp_for_eboot = fopen(WORKING_DIR "eboot.bin.elf","rb");
		assert(fp_for_eboot > 0);

		void *buf = memalign(4096, TRANSFER_SIZE);
		while ((eboot_chunk_size = fread(buf,1,TRANSFER_SIZE,fp_for_eboot)) > 0) {
			sha1_update(&ctx, buf, eboot_chunk_size);
		}
		sha1_final(&ctx, sha1_digest);
		fclose(fp_for_eboot);
	}
	if (args->offset_based_patch == OFFSET_BASED_PATCH_VITA_REMOVE_DLC_LOCKS) {
		const uint8_t LBP_VITA_1_22_STOCK_SHA1[SHA1_BLOCK_SIZE] = {0x34, 0x39, 0x39, 0xd3, 0x14, 0x65, 0x02, 0x17, 0xf9, 0xd3, 0xa5, 0x0b, 0x82, 0xa7, 0x22, 0x87, 0xe0, 0x7f, 0x07, 0x55};
		
		if (memcmp(sha1_digest,LBP_VITA_1_22_STOCK_SHA1,SHA1_BLOCK_SIZE) != 0) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_UNKNOWN_EBOOT);
			return THREAD_RET_UNKNOWN_EBOOT;
		}
		args->current_state = THREAD_CURRENT_STATE_PATCHING_VITA_DLC_UNLOCK;
		sceClibPrintf("Remove dlc lock patches vita\n");
		if (!remove_dlc_locks_vita_elf(WORKING_DIR "eboot.bin.elf")) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_EBOOT_PATCH_FAILED);
			return THREAD_RET_EBOOT_PATCH_FAILED;
		}	

	}

	args->current_state = THREAD_CURRENT_STATE_START_PATCHING;
	sceClibPrintf("start patching\n");
	clock_t start = clock();
	sprintf(lua_func_name,"patch_%s",args->patch_lua_name);
	
	// theese lua things might be unpure, something about me needing to pop the values after, but since it will always end after this idrc to do it
    lua_getglobal(L, lua_func_name);


	
	lua_pushstring(L,WORKING_DIR "eboot.bin.elf");
	lua_pushstring(L,my_url.url);
	lua_pushstring(L,my_url.digest);
	lua_pushboolean(L,args->normalise_digest);
	lua_pushstring(L,WORKING_DIR);
	lua_pushstring(L,args->join_password);
    if (lua_pcall(L, 6, 1, 0) != LUA_OK) {
        // gonna pop the error later
		// sceClibPrintf("Error calling function: %s\n", lua_tostring(L, -1));
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_PATCH_FAILED);
		return THREAD_RET_EBOOT_PATCH_FAILED;
    }
	// assuming that all errors are just raised now
	clock_t end = clock();
	args->current_state = THREAD_CURRENT_STATE_DONE_PATCHING;
	sceClibPrintf("done patching, took %f seconds\n",((double)(end - start)) / CLOCKS_PER_SEC);


	args->current_state = THREAD_CURRENT_STATE_EBOOT_ENCRYPT;
	sceClibPrintf("Encrypting eboot.bin (elf inject to working eboot.bin)\n");
	unmake_fself_res = elf_inject(WORKING_DIR "eboot.bin.elf",WORKING_DIR "eboot.bin");
	
	if (unmake_fself_res != 0) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_DECRYPT_FAILED);
		return THREAD_RET_EBOOT_DECRYPT_FAILED;
	}
	args->current_state = THREAD_CURRENT_STATE_MAKE_REPATCH_FOLDERS;
	sceClibPrintf("Making rePatch folders if not exist\n");
	
	
	
	mkdir("ux0:/rePatch/", 0777);
	mkdir(repatch_title_id_folder, 0777);
	args->current_state = THREAD_CURRENT_STATE_FINAL_COPY_EBOOT_TO_REPATCH;
	sceClibPrintf("Finally, copying new working eboot.bin to rePatch folder\n");
	copy_file_res = copy_file(repatch_eboot_bin_path,WORKING_DIR "eboot.bin");
	
	if (copy_file_res < 0) {
		args->has_finished = 1;
		sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
		return THREAD_RET_EBOOT_BACKUP_FAILED;
	}


	if (args->use_patch_cache) {
		// it is assumed at this point there is not cache for this eboot as thats checked for eariler
		char cache_eboot_bin_file[sizeof(CACHE_DIR "eboot_bin_repatch_vita_num_9999.bin")];
		sprintf(cache_eboot_bin_file,CACHE_DIR "eboot_bin_repatch_vita_num_%d.bin",line_num_of_caches);
		
		sceClibPrintf("Making cache copy of %s to %s",current_cache_line,cache_eboot_bin_file);
		
		copy_file_res = copy_file(cache_eboot_bin_file,repatch_eboot_bin_path);
		if (copy_file_res == -1) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
			return THREAD_RET_EBOOT_BACKUP_FAILED;
		}
		fp = fopen(CACHE_TXT_FILE, "ab+");
		if (fp == 0) {
			args->has_finished = 1;
			sceKernelExitThread(THREAD_RET_EBOOT_BACKUP_FAILED);
			return THREAD_RET_EBOOT_BACKUP_FAILED;
		}
		fprintf(fp,current_cache_line); fprintf(fp,"\n");
		
		fclose(fp);
	}

	args->has_finished = 1;
	sceKernelExitThread(THREAD_RET_EBOOT_PATCHED);
	return THREAD_RET_EBOOT_PATCHED;
}

int vita2d_font_draw_textf_with_bg(vita2d_font *font,u32 colour, u32 bg_colour,int x, int y, int size, char * text,...) {
	int x_from_font;
	char buf[1024];
	va_list argptr;

	va_start(argptr, text);
	vsnprintf(buf, sizeof(buf), text, argptr);
	va_end(argptr);
	
	// simple read by line
	char * line = buf;
	while (line) {
		char * next_line = strchr(line, '\n');
		if (next_line) {
			*next_line = '\0';
		}
		// per line code
		x_from_font = vita2d_font_draw_text(font, x, y, colour, size, line);
		vita2d_draw_rectangle(x, y-DRAW_CHAR_BG_COLOUR_HEIGHT, x_from_font, DRAW_CHAR_BG_COLOUR_HEIGHT, bg_colour);
		vita2d_font_draw_text(font, x, y, colour, size, line);
		y += CHARACTER_HEIGHT;
		// per line code end
		if (next_line) {
			*next_line = '\n';
		}
		line = next_line ? (next_line+1) : 0;
	}
	global_current_x = x_from_font;
	return x_from_font;
}

#define DrawFormatString(x, y, input_str, ...) \
    vita2d_font_draw_textf_with_bg(font, font_colour, bg_colour, x, y, TEXT_SIZE, input_str, ##__VA_ARGS__)

#define DrawFormatStringWithSize(size,x, y, input_str, ...) \
    vita2d_font_draw_textf_with_bg(font, font_colour, bg_colour, x, y, size, input_str, ##__VA_ARGS__)

#define DrawString(x, y, input_str) \
    vita2d_font_draw_textf_with_bg(font, font_colour, bg_colour, x, y, TEXT_SIZE, input_str)

#define U32_SWAP_FOR_COLOUR(num) ((u32)(((((u32)num) & 0xff000000) >> 24) | \
		   ((((u32)num) & 0x00ff0000) >> 8 ) | \
		   ((((u32)num) & 0x0000ff00) << 8 ) | \
		   ((((u32)num) & 0x000000ff) << 24)))

u32 internal_rainbow_colour_state = 0;
int internal_rainbow_colour_cur_index = 0;
/*
smooth transition for 

#FF0000
#00FF00
#0000FF
#00FFFF
#FF00FF

100 colours total, for end loop back to red at top
*/
const u32 rainbow_colours[] = {
	U32_SWAP_FOR_COLOUR(0xff0000ff),
	U32_SWAP_FOR_COLOUR(0xf20d00ff),
	U32_SWAP_FOR_COLOUR(0xe41b00ff),
	U32_SWAP_FOR_COLOUR(0xd72800ff),
	U32_SWAP_FOR_COLOUR(0xc93600ff),
	U32_SWAP_FOR_COLOUR(0xbc4300ff),
	U32_SWAP_FOR_COLOUR(0xae5100ff),
	U32_SWAP_FOR_COLOUR(0xa15e00ff),
	U32_SWAP_FOR_COLOUR(0x946b00ff),
	U32_SWAP_FOR_COLOUR(0x867900ff),
	U32_SWAP_FOR_COLOUR(0x798600ff),
	U32_SWAP_FOR_COLOUR(0x6b9400ff),
	U32_SWAP_FOR_COLOUR(0x5ea100ff),
	U32_SWAP_FOR_COLOUR(0x51ae00ff),
	U32_SWAP_FOR_COLOUR(0x43bc00ff),
	U32_SWAP_FOR_COLOUR(0x36c900ff),
	U32_SWAP_FOR_COLOUR(0x28d700ff),
	U32_SWAP_FOR_COLOUR(0x1be400ff),
	U32_SWAP_FOR_COLOUR(0x0df200ff),
	U32_SWAP_FOR_COLOUR(0x00ff00ff),
	U32_SWAP_FOR_COLOUR(0x00ff00ff),
	U32_SWAP_FOR_COLOUR(0x00f20dff),
	U32_SWAP_FOR_COLOUR(0x00e41bff),
	U32_SWAP_FOR_COLOUR(0x00d728ff),
	U32_SWAP_FOR_COLOUR(0x00c936ff),
	U32_SWAP_FOR_COLOUR(0x00bc43ff),
	U32_SWAP_FOR_COLOUR(0x00ae51ff),
	U32_SWAP_FOR_COLOUR(0x00a15eff),
	U32_SWAP_FOR_COLOUR(0x00946bff),
	U32_SWAP_FOR_COLOUR(0x008679ff),
	U32_SWAP_FOR_COLOUR(0x007986ff),
	U32_SWAP_FOR_COLOUR(0x006b94ff),
	U32_SWAP_FOR_COLOUR(0x005ea1ff),
	U32_SWAP_FOR_COLOUR(0x0051aeff),
	U32_SWAP_FOR_COLOUR(0x0043bcff),
	U32_SWAP_FOR_COLOUR(0x0036c9ff),
	U32_SWAP_FOR_COLOUR(0x0028d7ff),
	U32_SWAP_FOR_COLOUR(0x001be4ff),
	U32_SWAP_FOR_COLOUR(0x000df2ff),
	U32_SWAP_FOR_COLOUR(0x0000ffff),
	U32_SWAP_FOR_COLOUR(0x0000ffff),
	U32_SWAP_FOR_COLOUR(0x000dffff),
	U32_SWAP_FOR_COLOUR(0x001bffff),
	U32_SWAP_FOR_COLOUR(0x0028ffff),
	U32_SWAP_FOR_COLOUR(0x0036ffff),
	U32_SWAP_FOR_COLOUR(0x0043ffff),
	U32_SWAP_FOR_COLOUR(0x0051ffff),
	U32_SWAP_FOR_COLOUR(0x005effff),
	U32_SWAP_FOR_COLOUR(0x006bffff),
	U32_SWAP_FOR_COLOUR(0x0079ffff),
	U32_SWAP_FOR_COLOUR(0x0086ffff),
	U32_SWAP_FOR_COLOUR(0x0094ffff),
	U32_SWAP_FOR_COLOUR(0x00a1ffff),
	U32_SWAP_FOR_COLOUR(0x00aeffff),
	U32_SWAP_FOR_COLOUR(0x00bcffff),
	U32_SWAP_FOR_COLOUR(0x00c9ffff),
	U32_SWAP_FOR_COLOUR(0x00d7ffff),
	U32_SWAP_FOR_COLOUR(0x00e4ffff),
	U32_SWAP_FOR_COLOUR(0x00f2ffff),
	U32_SWAP_FOR_COLOUR(0x00ffffff),
	U32_SWAP_FOR_COLOUR(0x00ffffff),
	U32_SWAP_FOR_COLOUR(0x0df2ffff),
	U32_SWAP_FOR_COLOUR(0x1be4ffff),
	U32_SWAP_FOR_COLOUR(0x28d7ffff),
	U32_SWAP_FOR_COLOUR(0x36c9ffff),
	U32_SWAP_FOR_COLOUR(0x43bcffff),
	U32_SWAP_FOR_COLOUR(0x51aeffff),
	U32_SWAP_FOR_COLOUR(0x5ea1ffff),
	U32_SWAP_FOR_COLOUR(0x6b94ffff),
	U32_SWAP_FOR_COLOUR(0x7986ffff),
	U32_SWAP_FOR_COLOUR(0x8679ffff),
	U32_SWAP_FOR_COLOUR(0x946bffff),
	U32_SWAP_FOR_COLOUR(0xa15effff),
	U32_SWAP_FOR_COLOUR(0xae51ffff),
	U32_SWAP_FOR_COLOUR(0xbc43ffff),
	U32_SWAP_FOR_COLOUR(0xc936ffff),
	U32_SWAP_FOR_COLOUR(0xd728ffff),
	U32_SWAP_FOR_COLOUR(0xe41bffff),
	U32_SWAP_FOR_COLOUR(0xf20dffff),
	U32_SWAP_FOR_COLOUR(0xff00ffff),
	U32_SWAP_FOR_COLOUR(0xff00ffff),
	U32_SWAP_FOR_COLOUR(0xff00f2ff),
	U32_SWAP_FOR_COLOUR(0xff00e4ff),
	U32_SWAP_FOR_COLOUR(0xff00d7ff),
	U32_SWAP_FOR_COLOUR(0xff00c9ff),
	U32_SWAP_FOR_COLOUR(0xff00bcff),
	U32_SWAP_FOR_COLOUR(0xff00aeff),
	U32_SWAP_FOR_COLOUR(0xff00a1ff),
	U32_SWAP_FOR_COLOUR(0xff0094ff),
	U32_SWAP_FOR_COLOUR(0xff0086ff),
	U32_SWAP_FOR_COLOUR(0xff0079ff),
	U32_SWAP_FOR_COLOUR(0xff006bff),
	U32_SWAP_FOR_COLOUR(0xff005eff),
	U32_SWAP_FOR_COLOUR(0xff0051ff),
	U32_SWAP_FOR_COLOUR(0xff0043ff),
	U32_SWAP_FOR_COLOUR(0xff0036ff),
	U32_SWAP_FOR_COLOUR(0xff0028ff),
	U32_SWAP_FOR_COLOUR(0xff001bff),
	U32_SWAP_FOR_COLOUR(0xff000dff),
};



#define RAINBOW_LAST_INDEX (sizeof(rainbow_colours) / sizeof(rainbow_colours[0]))-1

u32 get_next_rainbow_colour() {
	struct timespec current_time;
	clock_gettime(CLOCK_REALTIME, &current_time);
	if (((u32)current_time.tv_nsec - internal_rainbow_colour_state) >= (1000000*10)) {
		internal_rainbow_colour_state = (u32)current_time.tv_nsec;
		internal_rainbow_colour_cur_index++;
	}
	if (internal_rainbow_colour_cur_index > RAINBOW_LAST_INDEX) {
		internal_rainbow_colour_cur_index = 0;
	}
	return rainbow_colours[internal_rainbow_colour_cur_index];
}

#define SetFontColor(font_colour_in,bg_colour_in) bg_colour = (bg_colour_in == 522001152) ? rainbow_colour : bg_colour_in; font_colour = (font_colour_in == 522001152) ? rainbow_colour : font_colour_in

#define GetFontX() global_current_x
void draw_scene(vita2d_font *font, u8 current_menu,int menu_arrow, bool is_alive_toggle_thing, u8 error_yet_to_press_ok, char* error_msg, int yes_no_game_popup, int started_a_thread, int thread_current_state,
u8 saved_urls_txt_num, bool normalise_digest_checked, bool use_patch_cache_checked, int offset_based_patch,
struct TitleIdAndGameName browse_games_buffer[], u32 browse_games_buffer_size, u32 browse_games_buffer_start,
char * global_title_id, int global_title_id_folder_type,
int method_count, struct LuaPatchDetails patch_lua_names[],
char * join_password
) {
	u32 rainbow_colour;
	int x_get_font;
	int x = 0;
	int y = CHARACTER_HEIGHT;
	u32 bg_colour;
	u32 font_colour;
	
	rainbow_colour = get_next_rainbow_colour();
	
	bg_colour = TITLE_BG_COLOUR;
	font_colour = TITLE_FONT_COLOUR;
	
	vita2d_set_clear_color(BACKGROUND_COLOUR);
	
	SetFontColor(TITLE_FONT_COLOUR, TITLE_BG_COLOUR);
	
	DrawFormatString(START_X_FOR_PRESS_TO_REFRESH_THINGS_TEXT,y,"Press "MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_TRIANGLE_BTN" to refresh things if ->%d<- is a solid 1 or 0, app is frozen " VERSION_NUM_STR,is_alive_toggle_thing);
	
	if (error_yet_to_press_ok != 0) {
		y += CHARACTER_HEIGHT;
		if (error_yet_to_press_ok == 1) {
			SetFontColor(ERROR_MESSAGE_COLOUR,ERROR_MESSAGE_BG_COLOUR);
		}
		else if (error_yet_to_press_ok == 2) {
			SetFontColor(SUCCESS_MESSAGE_COLOUR,SUCCESS_MESSAGE_BG_COLOUR);
		}
		DrawFormatString(x,y,error_msg);
		y += CHARACTER_HEIGHT*8; // give a bunch of space for title
		SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, SELECTED_FONT_BG_COLOUR);
		DrawFormatString(x,y,"Press %s to continue",MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CROSS_BTN);
		return;
	}
	
	else if (started_a_thread != 0) {
		y += CHARACTER_HEIGHT;
		switch (started_a_thread) {
			case YES_NO_GAME_POPUP_REVERT_EBOOT:
				DrawFormatString(x,y,"Reverting patches on your game. Please wait...");
				break;
			case YES_NO_GAME_POPUP_PATCH_GAME:
				DrawFormatString(x,y,"Applying patches to your game. Please wait...");
				y += CHARACTER_HEIGHT;
				bg_colour = TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);

				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_CLEANING_WORKSPACE) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Cleaning workspace");
				y += CHARACTER_HEIGHT;
				
				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_COPYING_FROM_FAGDEC) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Copying eboot.bin from FAGDec to workspace");
				y += CHARACTER_HEIGHT;
				
				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_EBOOT_DECRYPT) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Decrypting/decompressing eboot.bin in workspace");
				y += CHARACTER_HEIGHT;
				

				if (second_thread_args.offset_based_patch != 0) {
					bg_colour = (thread_current_state == THREAD_CURRENT_STATE_CALCING_EBOOT_ELF_SHA1) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
					SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
					DrawString(x, y, "Calculating sha1 checksum of eboot.bin.elf");
					y += CHARACTER_HEIGHT;
				}
				if (second_thread_args.offset_based_patch == OFFSET_BASED_PATCH_VITA_REMOVE_DLC_LOCKS) {
					bg_colour = (thread_current_state == THREAD_CURRENT_STATE_PATCHING_VITA_DLC_UNLOCK) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
					SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
					DrawString(x, y, "Removing DLC locks via offset seeking and writing");
					y += CHARACTER_HEIGHT;
				}

				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_START_PATCHING) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Patching eboot.bin.elf");
				y += CHARACTER_HEIGHT;

				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_DONE_PATCHING) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Done patching eboot.bin.elf");
				y += CHARACTER_HEIGHT;
				
				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_EBOOT_ENCRYPT) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Encrypting eboot.bin.elf to workspace eboot.bin");
				y += CHARACTER_HEIGHT;
				
				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_MAKE_REPATCH_FOLDERS) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Creating rePatch folders if not exist");
				y += CHARACTER_HEIGHT;
				
				bg_colour = (thread_current_state == THREAD_CURRENT_STATE_FINAL_COPY_EBOOT_TO_REPATCH) ? SELECTED_FONT_BG_COLOUR : TITLE_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawString(x, y, "Copying eboot.bin from workspace to rePatch folder");
				y += CHARACTER_HEIGHT;

				break;
			case YES_NO_GAME_POPUP_INSTALL_REPATCH:
				DrawFormatString(x,y,"Installing repatch_ex.skprx. Please wait...");
				break;
			case YES_NO_GAME_POPUP_REMOVE_ALLEFRESHER:
				DrawFormatString(x,y,"Removing Allefresher. Please wait...");
				break;
		}
		return;
	}
	
	else if (yes_no_game_popup != 0) {
		y += CHARACTER_HEIGHT;
		DrawFormatString(x,y,error_msg);
		y += CHARACTER_HEIGHT*8; // give a bunch of space for title

		bg_colour = (menu_arrow == 0) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
		SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
		DrawString(x,y,"Yes");
		y += CHARACTER_HEIGHT;

		bg_colour = (menu_arrow == 1) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
		SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
		DrawString(x,y,"No");
		y += CHARACTER_HEIGHT;

		return;
	}
	
    switch (current_menu) {
		case MENU_MAIN:
			
			DrawFormatString(x,y,"Main Menu");
			y += CHARACTER_HEIGHT*2;
			
			bg_colour = (menu_arrow == 0) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawString(x,y,"Select url");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 1) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawString(x,y,"Edit urls");
			y += CHARACTER_HEIGHT;


			bg_colour = (menu_arrow == 2) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawString(x,y,"Patch Games");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 3) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			font_colour = (use_patch_cache_checked) ? TURNED_ON_FONT_COLOUR : SELECTABLE_NORMAL_FONT_COLOUR;
			SetFontColor(font_colour, bg_colour);
			if (use_patch_cache_checked) {
				DrawString(x,y,"Patch cache: ON (turn OFF to clear cache)");
			}
			else {
				DrawString(x,y,"Patch cache: OFF (useful if you switch URLS often when ON)");
			}
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 4) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawString(x,y,"Exit");
			y += CHARACTER_HEIGHT;
			
			y += CHARACTER_HEIGHT*(3-1);
			
			SetFontColor(TURNED_ON_FONT_COLOUR,0);
			DrawString(x,y,"Things will have this font colour if it is selected");
			y += CHARACTER_HEIGHT*2;
			SetFontColor(TITLE_FONT_COLOUR,TITLE_BG_COLOUR);
			DrawFormatString(x,y,"Press %s to enter menus and select things",MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CROSS_BTN);
			y += CHARACTER_HEIGHT*2;
			DrawFormatString(x,y,"Press %s to go back to the previous menu",MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CIRCLE_BTN);
			y += CHARACTER_HEIGHT*2;
			DrawString(x,y,"Use the D-pad (up and down) to navigate through the menus\nleft and right to change pages");
			y += CHARACTER_HEIGHT*(2+1);
			DrawString(x,y,"Check out\nhttps://littlebigpatcherteam.github.io/2025/03/15/LBPC59548.html");
			y += CHARACTER_HEIGHT*(2+1);
			DrawString(x,y,"As per GPL-3.0 licence you MUST be provided the source code of this app!\nrefer to above for more info");
			break;
		case MENU_PATCH_GAMES:
			DrawFormatString(x,y,"Patch a game");
			y += CHARACTER_HEIGHT*2;
			
			bg_colour = (menu_arrow == 0) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawFormatString(x,y,"Edit Title id: ");
			DrawFormatString(GetFontX(),y,global_title_id);
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 1) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			font_colour = (normalise_digest_checked) ? TURNED_ON_FONT_COLOUR : SELECTABLE_NORMAL_FONT_COLOUR;
			SetFontColor(font_colour, bg_colour);
			DrawFormatString(x,y,"Normalise digest (select if debug build or previously patched by refresher)");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 2) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			font_colour = (offset_based_patch == OFFSET_BASED_PATCH_VITA_REMOVE_DLC_LOCKS) ? TURNED_ON_FONT_COLOUR : SELECTABLE_NORMAL_FONT_COLOUR;
			SetFontColor(font_colour, bg_colour);
			DrawFormatString(x,y,"Placeholder");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 3) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawFormatString(x,y,"Browse games for Title id");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 4) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawFormatString(x,y,"Revert patches");
			y += CHARACTER_HEIGHT;

			bg_colour = (menu_arrow == 5) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
			SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
			DrawFormatString(x,y,"Join key: ");
			if (join_password[0] == 0) {
				DrawFormatString(GetFontX(),y,"Randomised (no one can join you)");
			}
			else {
				DrawFormatString(GetFontX(),y,join_password);
			}
			
			y += CHARACTER_HEIGHT;

			for (int i = 0; i < method_count; i++) {
				bg_colour = (menu_arrow-MINUS_MENU_ARROW_AMNT_TO_GET_PATCH_LUA_INDEX == i) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
				SetFontColor(SELECTABLE_NORMAL_FONT_COLOUR, bg_colour);
				DrawFormatString(x,y,"Patch! (%s)",patch_lua_names[i].patch_method);
				y += CHARACTER_HEIGHT;
			}

			break;
			
		case MENU_SELECT_URLS:
		case MENU_EDIT_URLS:
			switch (current_menu) {
				case MENU_SELECT_URLS:
					DrawFormatString(x,y,"Select url\n(Page %d/99)",saved_urls_txt_num);
					break;
				case MENU_EDIT_URLS:
					DrawFormatString(x,y,"Edit urls\n(Page %d/99)",saved_urls_txt_num);
					break;
			}
			
			y += CHARACTER_HEIGHT*2;
			
			int i = 0;
			int current_url_entry_index;
			struct UrlToPatchTo url_entry;
			int full_text_len;
			int new_max_capitial_w_characters_per_line;
			int temp_new_x_len;
			//int temp_new_y_len;
			int i_stop = (current_menu == MENU_EDIT_URLS) ? saved_urls_count*2 : saved_urls_count;
			
			
			while (i < i_stop) {
				current_url_entry_index = i;
				if (current_menu == MENU_EDIT_URLS) {
					current_url_entry_index = i/2; // relying on round down, 3/2==1
				}
				url_entry = saved_urls[current_url_entry_index];

				
				
				bg_colour = (menu_arrow == i) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
				font_colour = (current_menu == MENU_SELECT_URLS && selected_url_index == i) ? TURNED_ON_FONT_COLOUR : SELECTABLE_NORMAL_FONT_COLOUR;
				SetFontColor(font_colour, bg_colour);
				
				
				full_text_len = strlen(url_entry.url) + 1 + strlen(url_entry.digest);
				temp_new_x_len = TEXT_SIZE;
				if (full_text_len > MAX_CAPITIAL_W_CHARACTERS_PER_LINE) {
					new_max_capitial_w_characters_per_line = MAX_CAPITIAL_W_CHARACTERS_PER_LINE;
					
					//temp_new_y_len = NORMAL_TEXT_Y;
					while (full_text_len > new_max_capitial_w_characters_per_line) {
						new_max_capitial_w_characters_per_line += NEW_LINES_AMNT_PER_DIGIT_OF_X_INCREASE; 
						temp_new_x_len -= 1;
						//temp_new_y_len -= 1;
					}
				}

				
				if (current_menu == MENU_EDIT_URLS) {
					if (i % 2 == 0) { // url i even case
						DrawFormatStringWithSize(temp_new_x_len,x,y,"%s",url_entry.url);
					}
					else { // digest i odd case
						DrawFormatStringWithSize(temp_new_x_len,GetFontX(),y," %s",url_entry.digest);
						y += CHARACTER_HEIGHT;
					}

				}
				else {
					DrawFormatStringWithSize(temp_new_x_len,x,y,"%s %s",url_entry.url,url_entry.digest);
					y += CHARACTER_HEIGHT;
				}

				i++;
			}
			break;
		case MENU_BROWSE_GAMES:
			DrawFormatString(x,y,"Browse games!\nTitle id: %s",global_title_id);
			y += CHARACTER_HEIGHT*2;
			DrawFormatString(x,y,"Game missing? ensure you have\nDECRYPT(SELF) on the game patch eboot.bin in FAGDec");
			y += CHARACTER_HEIGHT*2;
			for (int i = 0; i < browse_games_buffer_size; i++) {
				bg_colour = (menu_arrow == (i + browse_games_buffer_start)) ? SELECTED_FONT_BG_COLOUR : UNSELECTED_FONT_BG_COLOUR;
				font_colour = (strcmp(global_title_id,browse_games_buffer[i].title_id) == 0 && browse_games_buffer[i].title_id_folder_type == global_title_id_folder_type) ? TURNED_ON_FONT_COLOUR : SELECTABLE_NORMAL_FONT_COLOUR;
				SetFontColor(font_colour, bg_colour);
				if (browse_games_buffer[i].title_id_folder_type == TITLE_ID_PATCH) {
					DrawFormatString(x,y,"Patch: %s %s",browse_games_buffer[i].title_id,browse_games_buffer[i].game_name);
				}
				else if (browse_games_buffer[i].title_id_folder_type == TITLE_ID_APP) {
					DrawFormatString(x,y,"App: %s %s",browse_games_buffer[i].title_id,browse_games_buffer[i].game_name);
				}
				y += CHARACTER_HEIGHT;
			}
			
			break;
	}

}

int main(int argc, char *argv[]) {
	SceCtrlData pad;
	int my_btn;
	int old_btn = 0;
	int enter_button = SCE_SYSTEM_PARAM_ENTER_BUTTON_CROSS;
	char patch_or_app[sizeof("Patch")];
	bool is_alive_toggle_thing = 0;
	struct UrlToPatchTo temp_editing_url;
	char editing_url_text_buffer[72];
	u8 error_yet_to_press_ok = 0;
	bool exit_after_done = 0;
	bool reboot_the_vita = 0;
	int started_a_thread = 0;
	int yes_no_game_popup = 0;
	char * game_title;
	char param_sfo_path[sizeof("ux0:/patch/ABCD12345/sce_sys/param.sfo")];
	char error_msg[1000];
	
	char pretty_showey[500];
	bool has_done_a_switch = 1;
	u8 current_menu = MENU_MAIN;
	int menu_arrow = 0;
	u8 saved_urls_txt_num = 1;
	u32 browse_games_arrow = 0;
	u32 browse_games_buffer_start = 0;
	u32 browse_games_buffer_max_size = MAX_LINES-2; // minus 2 for the extra game missing msg, its 2 lines
	u32 browse_games_buffer_size = 0;
	struct TitleIdAndGameName browse_games_buffer[browse_games_buffer_max_size];
	
	int temp_title_id_folder_type;
	bool first_time = 1;
	int repatch_installed_res;
	
	int launch_by_uri_res;
	
	mkdir(ROOT_DIR, 0777);
	mkdir(WORKING_DIR, 0777);
	load_config();
	
	FILE *fp_to_write_placeholder;
	bool file_no_exist_or_is_empty = 0;
	
	fp_to_write_placeholder = fopen(NEW_NUM_1_SAVED_URLS_TXT,"rb");
	if (fp_to_write_placeholder == 0) {
		file_no_exist_or_is_empty = 1;
	}
	else {
		fseek(fp_to_write_placeholder, 0, SEEK_END);
		file_no_exist_or_is_empty = ftell(fp_to_write_placeholder) == 0;
		fclose(fp_to_write_placeholder);
	}

	if (file_no_exist_or_is_empty) {
		fp_to_write_placeholder = fopen(NEW_NUM_1_SAVED_URLS_TXT,"wb");
		if (fp_to_write_placeholder == 0) {
			return 1;
		}
		fwrite(DEFAULT_URLS,1,sizeof(DEFAULT_URLS)-1,fp_to_write_placeholder);
		fclose(fp_to_write_placeholder);
	}
	
	fp_to_write_placeholder = fopen(COLOUR_CONFIG_FILE,"rb");
	if (fp_to_write_placeholder == 0) {
		file_no_exist_or_is_empty = 1;
	}
	else {
		fseek(fp_to_write_placeholder, 0, SEEK_END);
		file_no_exist_or_is_empty = ftell(fp_to_write_placeholder) == 0;
		fclose(fp_to_write_placeholder);
	}
	
	if (file_no_exist_or_is_empty) {
		fp_to_write_placeholder = fopen(COLOUR_CONFIG_FILE,"wb");
		if (fp_to_write_placeholder == 0) {
			return 1;
		}
		fwrite(DEFAULT_COLOUR_CONFIG,1,sizeof(DEFAULT_COLOUR_CONFIG)-1,fp_to_write_placeholder);
		fclose(fp_to_write_placeholder);
	}


	mkdir(CACHE_DIR, 0777);
	
	fp_to_write_placeholder = fopen(PATCH_CACHE_FILE_EXISTS_THEN_TRUE,"rb");
	if (fp_to_write_placeholder != 0) {
		fclose(fp_to_write_placeholder);
	}
	
	second_thread_args.use_patch_cache = fp_to_write_placeholder != 0;


	// init the global second_thread_args
	second_thread_args.has_finished = 0;
	second_thread_args.current_state = 0;
	second_thread_args.normalise_digest = 1;
	second_thread_args.offset_based_patch = 0;
	second_thread_args.remove_allefresher = 0;
	memset(second_thread_args.patch_lua_name,0,sizeof(second_thread_args.patch_lua_name));
	second_thread_args.title_id_folder_type = global_title_id_folder_type;
	second_thread_args.title_id[0] = 0;
	memset(second_thread_args.join_password,0,4096+1);

	struct LuaPatchDetails patch_lua_names[MAX_LINES];
	int method_count = 0;
	int method_index = 0;
	char patch_method[sizeof(patch_lua_names[0].patch_method)];
	int lua_do_file_res;

	// thread vars
	SceUID second_thread_id;
	int second_thread_retval;
	void *second_args_pointer_to_avoid_copy = &second_thread_args;
	
	sceShellUtilInitEvents(0);

	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
	memset(&pad, 0, sizeof(pad));
	
	init_for_input();

	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, &enter_button);
	if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE) {
		BTN_CIRCLE = SCE_CTRL_CROSS;
		BTN_CROSS = SCE_CTRL_CIRCLE;
		strcpy(MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CROSS_BTN,"¹");
		strcpy(MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CIRCLE_BTN,"");
	}
	else {
		BTN_CIRCLE = SCE_CTRL_CIRCLE;
		BTN_CROSS = SCE_CTRL_CROSS;
		strcpy(MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CIRCLE_BTN,"¹");
		strcpy(MY_CUSTOM_EDIT_OF_NOTO_SANS_FONT_CROSS_BTN,"");
	}

	SceAppUtilAppEventParam eventParam;
	memset(&eventParam, 0, sizeof(SceAppUtilAppEventParam));
	sceAppUtilReceiveAppEvent(&eventParam);

	// lua setup
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_do_file_res = luaL_dofile(L, PATCH_LUA_FILE);
	if (lua_do_file_res) {
		sceClibPrintf("Error: %s\n", lua_tostring(L, -1));
		return 1;
	}
	
	// checking for patch functions
	char * func_name;
	int fun_name_len;

	int full_method_count = 0;
	char method_name_temp[sizeof(patch_lua_names[0].patch_name)+sizeof("patch_method_")];
	char * method_name_string_value_temp;
	
	lua_pushglobaltable(L);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (!lua_isfunction(L, -1)) {
			goto lua_pop_continue;
		}
		func_name = lua_tostring(L, -2);
		if (strncmp(func_name,"patch_",strlen("patch_")) != 0) {
			goto lua_pop_continue;
		}
		if (strncmp(func_name,"patch_ps3",strlen("patch_ps3")) == 0) {
			goto lua_pop_continue;
		}
		// dont accept `patch_` named things
		fun_name_len = strlen(func_name);
		if (fun_name_len == strlen("patch_")) {
			goto lua_pop_continue;
		}
		if (fun_name_len > PATCH_LUA_SIZE-strlen("patch_")) {
			goto lua_pop_continue;
		}
		if (method_count > MAX_LINES) {
			goto lua_pop_continue;
		}
		method_count++;
		strcpy(patch_lua_names[method_index].patch_name,func_name+strlen("patch_"));
		method_index++;
		lua_pop_continue:
		lua_pop(L, 1); 
	}
	lua_pop(L, 1); 

	for (method_index = 0; method_index < method_count; method_index++) {
		sprintf(method_name_temp,"patch_method_%s",patch_lua_names[method_index].patch_name);
		lua_getglobal(L, method_name_temp);
		if (lua_isstring(L, -1)) {
			method_name_string_value_temp = lua_tostring(L, -1);
			if (strlen(method_name_string_value_temp) > sizeof(patch_lua_names[method_index].patch_method)-1) {
				continue;
			}
			strcpy(patch_lua_names[method_index].patch_method,method_name_string_value_temp);
			full_method_count++;
		}
		lua_pop(L, 1);
	}
	if (full_method_count != method_count) {
		sceClibPrintf("found some functions but they had no patch_method_ string for it\n");
		return 1;
	}
	lua_pushboolean(L, 0);
	lua_setglobal(L, "IS_BIG_ENDIAN");

	method_index = 0;
	
	vita2d_init();
	vita2d_font *font = vita2d_load_font_mem(NotoSans_Regular_ttf_bin, NotoSans_Regular_ttf_bin_len);

	while (1) {
		my_btn = get_button_pressed(&pad);
		vita2d_start_drawing();
		vita2d_clear_screen();
		
		if (first_time) {
			if (LIVEAREA_SELECTED("psla:-install_fagdec_vpk",eventParam)) {
				FILE *vita_shell_lastdir = fopen("ux0:/VitaShell/internal/lastdir.txt","wb");
				if (vita_shell_lastdir == 0) {
					sprintf(error_msg,"Looks like VitaShell isn't installed, please install it first and boot it at least once");
					exit_after_done = 1;
					goto done_with_first_time_checking;
				}
				fwrite(FAGDEC_VPK_LOCATION,1,sizeof(FAGDEC_VPK_LOCATION),vita_shell_lastdir);
				fclose(vita_shell_lastdir);
				launch_by_uri_res = sceAppMgrLaunchAppByUri(0x20000, "psgm:play?titleid=VITASHELL");
				if (launch_by_uri_res < 0) {
					sprintf(error_msg,"Some reason, we could not boot VitaShell");
					exit_after_done = 1;
					goto done_with_first_time_checking;
				}
				return 0;
				
			}
			repatch_installed_res = is_repatch_installed();
			if (is_repatch_installed() < 0) {
				error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
				sprintf(error_msg,"Some reason, we could not parse your tai config.txt, best to report this error");
				exit_after_done = 1;
				goto done_with_first_time_checking;
			}
			if (repatch_installed_res == REPATCH_INSTALLED_REPATCH) {

			}
			else if (repatch_installed_res == REPATCH_INSTALLED_ALLEFRESHER) {
				yes_no_game_popup = YES_NO_GAME_POPUP_REMOVE_ALLEFRESHER;
				sprintf(error_msg,"Allefresher was detected to be installed, this needs to be removed for this to work\nWould you like to remove Allefresher (would require a reboot)");
				menu_arrow = 0; // default select yes because we want users to do this
				goto done_with_first_time_checking;
			}
			else if (repatch_installed_res == 0) {
				yes_no_game_popup = YES_NO_GAME_POPUP_INSTALL_REPATCH;
				sprintf(error_msg,"rePatch is not detected, this is required for this app to work\nWould you like to install rePatch? (would require a reboot)");
				menu_arrow = 0; // default select yes because we want users to do this
				goto done_with_first_time_checking;
			}
			done_with_first_time_checking:
			first_time = 0;
			goto draw_scene_direct;
		}

		
 		if (!(my_btn & old_btn)) {
			// special menus, popups
			if (error_yet_to_press_ok) {
				if (my_btn & BTN_CROSS) {
					if (exit_after_done) {
						return 0;
					}
					if (reboot_the_vita) {
						scePowerRequestColdReset();
						return 0;
					}
					error_yet_to_press_ok = 0;
				}
				goto draw_scene_direct;
			}
			
			else if (started_a_thread) {
				if (second_thread_args.has_finished) {
					assert(sceKernelWaitThreadEnd(second_thread_id,&second_thread_retval,0) == 0);
					assert(sceKernelDeleteThread(second_thread_id) == 0);
					second_thread_args.has_finished = 0;
					started_a_thread = 0;
					
					switch (second_thread_retval) {
						case THREAD_RET_EBOOT_REVERTED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_SUCCESS;
							sprintf(error_msg,"Succesfully reverted patches on %s",global_title_id);
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							goto draw_scene_direct;
							break;

						case THREAD_RET_EBOOT_BAK_NO_EXIST:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							sprintf(error_msg,"ux0:rePatch/%s/eboot.bin does not exist\nmost likley you never patched before",global_title_id);
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							goto draw_scene_direct;
							break;
						
						case THREAD_RET_EBOOT_PATCHED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_SUCCESS;
							sprintf(error_msg,"Succesfully patched (%s)%s",patch_method,pretty_showey);
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_EBOOT_BACKUP_FAILED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							sprintf(error_msg,"Some reason, we could not backup your eboot.bin on %s",pretty_showey);
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_EBOOT_DECRYPT_FAILED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							sprintf(error_msg,"Could not decrypt eboot.bin on%s",pretty_showey);
							
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_EBOOT_PATCH_FAILED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							sprintf(error_msg,"Could not patch (%s) eboot.bin on%s\n%s\nplease report your game",patch_method,pretty_showey,lua_tostring(L, -1)+(strlen(PATCH_LUA_FILE)-strlen(PATCH_LUA_FILE_NAME)));
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_UNKNOWN_EBOOT:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							sprintf(error_msg,"Missmatching eboot.elf on%s\nIf you think this is wrong, please report your game",pretty_showey);
							current_menu = MENU_PATCH_GAMES;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_REPATCH_INSTALL_FAILED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							if (second_thread_args.remove_allefresher) {
								sprintf(error_msg,"Some reason, we could not remove Allefresher, best to report this");
							}
							else {
								sprintf(error_msg,"Some reason, we could not install rePatch, best to report this");
							}
							current_menu = MENU_MAIN;
							menu_arrow = 0;
							exit_after_done = 1;
							goto draw_scene_direct;
							break;
						case THREAD_RET_REPATCH_INSTALLED:
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_SUCCESS;
							if (second_thread_args.remove_allefresher) {
								sprintf(error_msg,"Allefresher removed successfully! continue to reboot!");
							}
							else {
								sprintf(error_msg,"rePatch installed successfully! continue to reboot!");
							}
							current_menu = MENU_MAIN;
							menu_arrow = 0;
							sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN); reboot_the_vita = 1;
							goto draw_scene_direct;
							break;
						default:
							assert(0);
					}
					
				}
				goto draw_scene_direct;
			}
			
			else if (yes_no_game_popup != 0) {
				if (my_btn & BTN_CROSS) {
					if (menu_arrow == 1) {
						
					}
					else {
						switch (yes_no_game_popup) {
							case YES_NO_GAME_POPUP_REVERT_EBOOT:
								strcpy(second_thread_args.title_id,global_title_id);
								second_thread_args.title_id_folder_type = global_title_id_folder_type;
								second_thread_id = sceKernelCreateThread(SECOND_THREAD_NAME, revert_patches_thread, SECOND_THREAD_PRIORITY, SECOND_THREAD_STACK_SIZE, 0, 0, NULL);
								assert(second_thread_id > 0);
								assert(sceKernelStartThread(second_thread_id, sizeof(second_thread_args), &second_args_pointer_to_avoid_copy) == 0);
								started_a_thread = YES_NO_GAME_POPUP_REVERT_EBOOT;
								break;
							case YES_NO_GAME_POPUP_PATCH_GAME:
								strcpy(second_thread_args.title_id,global_title_id);
								second_thread_args.title_id_folder_type = global_title_id_folder_type;
								second_thread_id = sceKernelCreateThread(SECOND_THREAD_NAME, apply_patches_thread, SECOND_THREAD_PRIORITY, SECOND_THREAD_STACK_SIZE, 0, 0, NULL);
								assert(second_thread_id > 0);
								assert(sceKernelStartThread(second_thread_id, sizeof(second_thread_args), &second_args_pointer_to_avoid_copy) == 0);
								started_a_thread = YES_NO_GAME_POPUP_PATCH_GAME;
								break;
							case YES_NO_GAME_POPUP_INSTALL_REPATCH:
								second_thread_args.remove_allefresher = 0;
								second_thread_id = sceKernelCreateThread(SECOND_THREAD_NAME, install_repatch_thread, SECOND_THREAD_PRIORITY, SECOND_THREAD_STACK_SIZE, 0, 0, NULL);
								assert(second_thread_id > 0);
								assert(sceKernelStartThread(second_thread_id, sizeof(second_thread_args), &second_args_pointer_to_avoid_copy) == 0);
								started_a_thread = YES_NO_GAME_POPUP_INSTALL_REPATCH;
								break;
							case YES_NO_GAME_POPUP_REMOVE_ALLEFRESHER:
								second_thread_args.remove_allefresher = 1;
								second_thread_id = sceKernelCreateThread(SECOND_THREAD_NAME, install_repatch_thread, SECOND_THREAD_PRIORITY, SECOND_THREAD_STACK_SIZE, 0, 0, NULL);
								assert(second_thread_id > 0);
								assert(sceKernelStartThread(second_thread_id, sizeof(second_thread_args), &second_args_pointer_to_avoid_copy) == 0);
								started_a_thread = YES_NO_GAME_POPUP_REMOVE_ALLEFRESHER;
								break;
							default:
								assert(0);
						}
					}
					yes_no_game_popup = 0;
					menu_arrow = 0;
				}
				menu_arrow = set_arrow(menu_arrow,my_btn,YES_NO_POPUP_ARROW);
				goto draw_scene_direct;
			}
			// refresh for global_title_id anywhere, can also put other things that refresh should refresh everywhere
			if (my_btn & BTN_TRIANGLE) {
				DONE_A_SWITCH;
				load_config();
				menu_arrow = 0;
			}
			
			// This might be differnt depdning on what menu but for now itll suffice
			if (my_btn & BTN_CIRCLE) {
				DONE_A_SWITCH;
				menu_arrow = 0;
				if (current_menu == MENU_BROWSE_GAMES) {
					current_menu = MENU_PATCH_GAMES;
				}
				else {
					current_menu = MENU_MAIN;
				}
			}
			// normal menus
			if (my_btn & BTN_RIGHT || my_btn & BTN_LEFT) {
				switch (current_menu) {
					case MENU_SELECT_URLS:
					case MENU_EDIT_URLS:
						DONE_A_SWITCH;
						selected_url_index = RESET_SELECTED_URL_INDEX;
						if (my_btn & BTN_RIGHT) {
							if (saved_urls_txt_num >= 99) {
								saved_urls_txt_num = 1;
							}
							else {
								saved_urls_txt_num++;
							}
						}
						else if (my_btn & BTN_LEFT) {
							if (saved_urls_txt_num <= 1) {
								saved_urls_txt_num = 99;
							}
							else {
								saved_urls_txt_num--;
							}
						}
						break;
				}
			}
			if (my_btn & BTN_CROSS) {
				DONE_A_SWITCH;
				switch (current_menu) {
					case MENU_MAIN:
						switch (menu_arrow) {
							case 0:
								current_menu = MENU_SELECT_URLS;
								break;
							case 1:
								current_menu = MENU_EDIT_URLS;
								break;
							case 2:
								current_menu = MENU_PATCH_GAMES;
								break;
							case 3:
								second_thread_args.use_patch_cache = !second_thread_args.use_patch_cache;
								save_patch_cache_bool(second_thread_args.use_patch_cache);
								break;
							case 4:
								return 0; // exit
								break;
						}
						break;
					case MENU_PATCH_GAMES:
						switch (menu_arrow) {
							case 0:
								while (1) {
									input("Enter in a title id (example PCSF00021)",global_title_id,sizeof(global_title_id));
									for (int i = 0; global_title_id[i] != '\0'; i++) {
										global_title_id[i] = toupper(global_title_id[i]);
									}
									if (is_valid_title_id(global_title_id)) {
										save_global_title_id_to_disk();
										break;
									}
									
								}
								break;
							case 1:
								second_thread_args.normalise_digest = !second_thread_args.normalise_digest;
								break;
							case 2:
								if (second_thread_args.offset_based_patch == OFFSET_BASED_PATCH_VITA_REMOVE_DLC_LOCKS) {
									second_thread_args.offset_based_patch = 0;
								}
								else {
									second_thread_args.offset_based_patch = 0;
								}
								break;
							case 3:
								current_menu = MENU_BROWSE_GAMES;
								menu_arrow = 0;
								break;
							case 5:
								input("Enter in join key (leave empty if only play alone) (follow best pwd practices!)",second_thread_args.join_password,sizeof(second_thread_args.join_password));
								save_user_join_pwd(second_thread_args.join_password);
								break;
							default:
								
								if (!(temp_title_id_folder_type = title_id_exists(global_title_id))) {
									error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
									sprintf(error_msg,"We could not find %s, is the title id correct? ensure you have\nDECRYPT(SELF) on the game patch eboot.bin in FAGDec",global_title_id);
									current_menu = MENU_PATCH_GAMES;
									menu_arrow = 0;
									goto draw_scene_direct;
								}
								if (global_title_id_folder_type == 0) {
									global_title_id_folder_type = temp_title_id_folder_type;
								}
								struct UrlToPatchTo temp_show = saved_urls[selected_url_index];
								if (global_title_id_folder_type == TITLE_ID_PATCH) {
									strcpy(patch_or_app,"Patch");
									sprintf(param_sfo_path,"ux0:/patch/%s/sce_sys/param.sfo",global_title_id);
								}
								else if (global_title_id_folder_type == TITLE_ID_APP) {
									strcpy(patch_or_app,"App");
									sprintf(param_sfo_path,"ux0:/app/%s/sce_sys/param.sfo",global_title_id);
								}
								
								game_title = get_title_id_from_param(param_sfo_path);
								if (game_title == 0 ) {
									game_title = malloc(sizeof("Unknown??"));
									strcpy(game_title,"Unknown??");
								}
								if (temp_show.digest[0]) {
									sprintf(pretty_showey,"\n%s\nType: %s Title id: %s\nwith the url\n%s\nand with digest key\n%s",game_title,patch_or_app,global_title_id,temp_show.url,temp_show.digest);
								}
								else {
									sprintf(pretty_showey,"\n%s\nType: %s Title id: %s\nwith the url\n%s",game_title,patch_or_app,global_title_id,temp_show.url);
								}
								
								if (menu_arrow == 4) {
									yes_no_game_popup = YES_NO_GAME_POPUP_REVERT_EBOOT;
									sprintf(error_msg,"Do you want to revert patches on\n%s\nTitle id: %s",game_title,global_title_id);
								}
								
								else {
									method_index = menu_arrow - MINUS_MENU_ARROW_AMNT_TO_GET_PATCH_LUA_INDEX;
									strcpy(patch_method,patch_lua_names[method_index].patch_method);
									strcpy(second_thread_args.patch_lua_name,patch_lua_names[method_index].patch_name);
									yes_no_game_popup = YES_NO_GAME_POPUP_PATCH_GAME;
									sprintf(error_msg,"Do you want to patch (%s)%s",patch_method,pretty_showey);
								}
								free(game_title);
								
								current_menu = MENU_PATCH_GAMES;
								menu_arrow = 1;
								goto draw_scene_direct;
								break;
						}
						break;
					case MENU_SELECT_URLS:
						selected_url_index = (menu_arrow == selected_url_index) ? RESET_SELECTED_URL_INDEX : menu_arrow;
						break;
					case MENU_EDIT_URLS:
						selected_url_index = RESET_SELECTED_URL_INDEX;
						temp_editing_url = saved_urls[menu_arrow/2];
						
						if (menu_arrow % 2 == 0) { // url menu_arrow even case
							strcpy(editing_url_text_buffer,temp_editing_url.url);
							input("Enter in a URL",editing_url_text_buffer,sizeof(temp_editing_url.url));
							
							strcpy(saved_urls[menu_arrow/2].url,editing_url_text_buffer);
							strcpy(saved_urls[menu_arrow/2].digest,temp_editing_url.digest);
						}
						else { // digest menu_arrow odd case
							strcpy(editing_url_text_buffer,temp_editing_url.digest);
							input("Enter in a digest key, put in CustomServerDigest if this is a refresh server otherwise leave empty",editing_url_text_buffer,sizeof(temp_editing_url.digest));
							strcpy(saved_urls[menu_arrow/2].digest,editing_url_text_buffer);
							strcpy(saved_urls[menu_arrow/2].url,temp_editing_url.url);
						}
						
						write_saved_urls(saved_urls_txt_num);
						
						break;
					case MENU_BROWSE_GAMES:
						strcpy(global_title_id,browse_games_buffer[menu_arrow - browse_games_buffer_start].title_id);
						global_title_id_folder_type = browse_games_buffer[menu_arrow - browse_games_buffer_start].title_id_folder_type;
						save_global_title_id_to_disk();	
						break;
				}
				// put code here if you dont want the menu arrow to reset
				if (current_menu == MENU_BROWSE_GAMES) {
				
				}
				else {
					menu_arrow = 0;
				}
			}
			switch (current_menu) {
				case MENU_MAIN:
					if (has_done_a_switch) {
						// do first time code here
						has_done_a_switch = 0;
					}
					
					menu_arrow = set_arrow(menu_arrow,my_btn,MENU_MAIN_ARROW);

					break;
				
				case MENU_PATCH_GAMES:
					if (has_done_a_switch) {
						// do first time code here
						if (!is_a_url_selected()) {
							error_yet_to_press_ok = ERROR_YET_TO_PRESS_OK_FAIL;
							strcpy(error_msg,"Please select a url in Select Url menu");
							current_menu = MENU_MAIN;
							menu_arrow = 0;
							goto draw_scene_direct;
						}
						
						has_done_a_switch = 0;
					}
					
					menu_arrow = set_arrow(menu_arrow,my_btn,MENU_PATCH_GAMES_ARROW);

					break;
				
				case MENU_EDIT_URLS:
				case MENU_SELECT_URLS:
					if (has_done_a_switch) {
						load_saved_urls(saved_urls_txt_num);
						
						has_done_a_switch = 0;
					}
					if (current_menu == MENU_EDIT_URLS) {
						menu_arrow = set_arrow(menu_arrow,my_btn,MENU_EDIT_URLS_ARROW);
					}
					else {
						menu_arrow = set_arrow(menu_arrow,my_btn,MENU_SELECT_URLS_ARROW);
					}
					break;
				
				case MENU_BROWSE_GAMES:
					if (has_done_a_switch) {
						browse_games_arrow = total_count_of_patchable_games(0,0xFFFFFFFF) - 1;
						if (menu_arrow == 0) {
							browse_games_buffer_start = 0;
							browse_games_buffer_size = load_patchable_games(browse_games_buffer,browse_games_buffer_start,browse_games_buffer_max_size);
						}
						has_done_a_switch = 0;
					}
					menu_arrow = set_arrow(menu_arrow,my_btn,browse_games_arrow);
					if (menu_arrow < browse_games_buffer_start) {
						browse_games_buffer_start -= browse_games_buffer_max_size;
						browse_games_buffer_size = load_patchable_games(browse_games_buffer,browse_games_buffer_start,browse_games_buffer_max_size);
					}
					else if (menu_arrow > (browse_games_buffer_start + browse_games_buffer_max_size)-1) {
						browse_games_buffer_start += browse_games_buffer_max_size;
						browse_games_buffer_size = load_patchable_games(browse_games_buffer,browse_games_buffer_start,browse_games_buffer_max_size);
					}
					
					break;

			}

		}
		draw_scene_direct:
		old_btn = my_btn;
		draw_scene(font,current_menu,menu_arrow,is_alive_toggle_thing,error_yet_to_press_ok,error_msg,yes_no_game_popup,
		started_a_thread,second_thread_args.current_state,saved_urls_txt_num,second_thread_args.normalise_digest,second_thread_args.use_patch_cache,
		second_thread_args.offset_based_patch,
		browse_games_buffer,browse_games_buffer_size,browse_games_buffer_start,global_title_id,global_title_id_folder_type,
		method_count,patch_lua_names, second_thread_args.join_password);
		is_alive_toggle_thing = !is_alive_toggle_thing;
		
        vita2d_end_drawing();
        vita2d_swap_buffers();
	}
	vita2d_fini();
	vita2d_free_font(font);
	return 0;
}
