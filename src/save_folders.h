#ifndef SAVE_FOLDERS_H_   /* Include guard */
#define SAVE_FOLDERS_H_

#define VERSION_NUM_STR "v2.010"

#define ROOT_DIR "ux0:/data/lbpatcher_stuff/"
#define WORKING_DIR ROOT_DIR "temp_files/"
#define COLOUR_CONFIG_FILE ROOT_DIR "colours_config.txt"
#define PATCH_LUA_FILE_NAME "patch.lua"
#define PATCH_LUA_FILE "app0:/" PATCH_LUA_FILE_NAME


#define DEFAULT_URLS "http://lighthouse.lbpunion.com/LITTLEBIGPLANETPS3_XML\n"\
					 "http://refresh.jvyden.xyz:2095/lbp CustomServerDigest\n"\
					 "http://lnfinite.site/LITTLEBIGPLANETPS3_XML\n"\

#define DEFAULT_COLOUR_CONFIG "// they are rgba hex colour codes, not rgb hex colour codes, so if you want red FF0000 for example, put in #FF0000FF, othwise it wont be visible\n"\
							  "TITLE_FONT_COLOUR #ffffffff // white\n"\
							  "TITLE_BG_COLOUR #2b2600ff // some ugly moss green\n"\
							  "SELECTABLE_NORMAL_FONT_COLOUR #ffffffff // white\n"\
							  "TURNED_ON_FONT_COLOUR #FFFF00ff // yellow\n"\
							  "BACKGROUND_COLOUR #002b26ff // very dark turquiose\n"\
							  "SELECTED_FONT_BG_COLOUR #575757ff // dark grey\n"\
							  "UNSELECTED_FONT_BG_COLOUR #00000000 // invisible\n"\
							  "ERROR_MESSAGE_COLOUR #FF0000FF // red\n"\
							  "ERROR_MESSAGE_BG_COLOUR #00000000 // invisible\n"\
							  "SUCCESS_MESSAGE_COLOUR #00FF00FF // green\n"\
							  "SUCCESS_MESSAGE_BG_COLOUR #00000000 // invisible\n"\

#define OLD_SAVED_URLS_TXT ROOT_DIR "saved_urls.txt"
#define NEW_NUM_1_SAVED_URLS_TXT ROOT_DIR "saved_urls_1.txt"
#define SAVED_URLS_TXT_FIRST_HALF ROOT_DIR "saved_urls"
#define SAVED_URLS_TXT_SECOND_HALF ".txt"

#define FAGDEC_VPK_LOCATION "ux0:/app/LBPC59548/fagdec_vpk/"

#define BUILT_IN_REPATCH_SKPRX_PATH "app0:/repatch_ex.skprx"
#define TEMP_TAI_CONFIG WORKING_DIR "tai_config.txt"
#define REPATCH_SKPRX_UR0_ENTRY "ur0:tai/repatch_ex.skprx\n"
#define REPATCH_SKPRX_UX0_ENTRY "ux0:tai/repatch_ex.skprx\n"
#define REPATCH_SKPRX_UR0_PATH "ur0:/tai/repatch_ex.skprx"
#define REPATCH_SKPRX_UX0_PATH "ux0:/tai/repatch_ex.skprx"

#define TITLE_ID_TXT ROOT_DIR "title_id_to_patch.txt"


#endif // SAVE_FOLDERS_H_
