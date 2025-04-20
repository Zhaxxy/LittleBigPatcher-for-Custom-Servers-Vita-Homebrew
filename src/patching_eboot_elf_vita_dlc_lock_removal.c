/*
based off 
https://gist.github.com/Zhaxxy/8bc5994881678ef15739fd6d51465b3b
*/
#include "types_for_my_patcher_app.h"
#include "stdio.h"

#define BASE_EBOOT_BIN_FILE_CODE_OFFSET 0xA0
#define BASE_IN_MEMORY_CODE_OFFSET 0x81000000

#define MEMORY_OFFSET_TO_FILE_OFFSET(memory_offset) (memory_offset - BASE_IN_MEMORY_CODE_OFFSET) + BASE_EBOOT_BIN_FILE_CODE_OFFSET

// 2 NOPs, to remove the close call as to not cause issues
const uint8_t ARM_THUMB_TWO_NOPS[4] = "\x00\xbf\x00\xbf";

// nop essentially means jump never
const uint8_t ARM_THUMB_NOP[2] = "\x00\xbf";

// make it not call anything and just return 0, meaning success
const uint8_t ARM_THUMB_EOR_R0_R0_R0[4] = "\x80\xea\x00\x00";

bool remove_dlc_locks_vita_elf(const char * input_elf) {
    FILE *fp = fopen(input_elf,"rb+");
    if (fp == 0)  {
        return 0;
    }
    // network pass DLC lock removal

    // sceAppUtilDrmOpen call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x81246e6a),SEEK_SET);
    fwrite(ARM_THUMB_EOR_R0_R0_R0,1,sizeof(ARM_THUMB_EOR_R0_R0_R0),fp);

    // sceAppUtilDrmClose call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x81246e76),SEEK_SET);
    fwrite(ARM_THUMB_TWO_NOPS,1,sizeof(ARM_THUMB_TWO_NOPS),fp);

    // sceAppUtilDrmOpen call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x812465f2),SEEK_SET);
    fwrite(ARM_THUMB_EOR_R0_R0_R0,1,sizeof(ARM_THUMB_EOR_R0_R0_R0),fp);

    // sceAppUtilDrmClose call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x812465fe),SEEK_SET);
    fwrite(ARM_THUMB_TWO_NOPS,1,sizeof(ARM_THUMB_TWO_NOPS),fp);


    // other DLCs lock removal

    // sceAppUtilDrmOpen call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117a9a8),SEEK_SET);
    fwrite(ARM_THUMB_EOR_R0_R0_R0,1,sizeof(ARM_THUMB_EOR_R0_R0_R0),fp);

    // sceAppUtilDrmClose call
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117aaf0),SEEK_SET);
    fwrite(ARM_THUMB_TWO_NOPS,1,sizeof(ARM_THUMB_TWO_NOPS),fp);

    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117a9fc),SEEK_SET);
    fwrite(ARM_THUMB_NOP,1,sizeof(ARM_THUMB_NOP),fp);

    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117aa16),SEEK_SET);
    fwrite(ARM_THUMB_NOP,1,sizeof(ARM_THUMB_NOP),fp);
    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117aa1c),SEEK_SET);
    fwrite(ARM_THUMB_NOP,1,sizeof(ARM_THUMB_NOP),fp);

    fseek(fp,MEMORY_OFFSET_TO_FILE_OFFSET(0x8117aa64),SEEK_SET);
    fwrite(ARM_THUMB_NOP,1,sizeof(ARM_THUMB_NOP),fp);

    fclose(fp);
    return 1;
}
