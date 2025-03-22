#include <stdio.h>

#include "types_for_my_patcher_app.h"
#include "patching_eboot_elf_code.h"

#define PATCH_LUA_SIZE 256
#define PATCH_METHOD_LUA_STRING_SIZE 512

struct SecondThreadArgs {
    bool has_finished;
	int current_state;
	bool normalise_digest;
	int offset_based_patch;
	bool remove_allefresher;
	char patch_lua_name[PATCH_LUA_SIZE];
	int title_id_folder_type;
    char title_id[sizeof("BCES12345")];
};

struct SecondThreadArgs second_thread_args;

char global_content_id[128];

bool does_file_exist(char * filename) {
	FILE *fp = fopen(filename,"rb");
	
	if (fp == 0) {
		return 0;
	}
	fclose(fp);
	return 1;
}