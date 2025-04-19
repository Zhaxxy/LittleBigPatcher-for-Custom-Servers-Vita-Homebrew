#ifndef FOR_ELFINJECT_GLOBALS_H_
#define FOR_ELFINJECT_GLOBALS_H_

#include "types_for_my_patcher_app.h"

#define PATCH_LUA_SIZE 256
#define PATCH_METHOD_LUA_STRING_SIZE 512

#define OFFSET_BASED_PATCH_VITA_REMOVE_DLC_LOCKS 1

bool does_file_exist(char * filename);

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

extern struct SecondThreadArgs second_thread_args;
extern bool global_found_raps;
extern bool global_is_digital_eboot;
extern char global_content_id[128];

#endif // FOR_ELFINJECT_GLOBALS_H_