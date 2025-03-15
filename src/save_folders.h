#ifndef SAVE_FOLDERS_H_   /* Include guard */
#define SAVE_FOLDERS_H_

#define VERSION_NUM_STR "v2.000"

#define ROOT_DIR "ux0:/data/lbpatcher_stuff/"
#define WORKING_DIR ROOT_DIR "temp_files/"
#define COLOUR_CONFIG_FILE ROOT_DIR "colours_config.txt"


#define DEFAULT_URLS "http://lighthouse.lbpunion.com/LITTLEBIGPLANETPS3_XML\n"\
					 "http://refresh.jvyden.xyz:2095/lbp CustomServerDigest\n"\
					 "http://lnfinite.site/LITTLEBIGPLANETPS3_XML\n"\

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