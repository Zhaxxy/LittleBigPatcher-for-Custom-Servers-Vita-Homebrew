## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 3.2)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

## Define project parameters here
# Name of the project
project(LittleBigPatcher_for_Custom_Servers_Vita_Homebrew)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "LittleBigPatcher for Custom Servers (Vita Homebrew)")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "LBPC59548")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.00")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

# Add any additional include paths here
include_directories(

)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

## Build and link
# Add all the files needed to compile here
add_executable(${PROJECT_NAME}
  src/main.c
  src/colours_config.c
  src/read_sfo.c
  src/for_elfinject_globals.c
  src/text_input_vita.c
  src/copyfile_thing.c

  src/vita-unmake-fself/vita_unmake_fself.c
  
  src/elf_injector/elf_inject.c
  src/elf_injector/sha256.c
  
  # i had to edit the source/lua-5.4.7/src a bit, rename the main functions in lua.c and luac.c
  # from https://www.lua.org/ftp/lua-5.4.7.tar.gz
  src/lua-5.4.7/src/ldebug.c
  src/lua-5.4.7/src/lcorolib.c
  src/lua-5.4.7/src/loslib.c
  src/lua-5.4.7/src/lmem.c
  src/lua-5.4.7/src/lauxlib.c
  src/lua-5.4.7/src/lzio.c
  src/lua-5.4.7/src/lctype.c
  src/lua-5.4.7/src/lstring.c
  src/lua-5.4.7/src/ltm.c
  src/lua-5.4.7/src/lparser.c
  src/lua-5.4.7/src/lstrlib.c
  src/lua-5.4.7/src/llex.c
  src/lua-5.4.7/src/ltable.c
  src/lua-5.4.7/src/lobject.c
  src/lua-5.4.7/src/linit.c
  src/lua-5.4.7/src/lvm.c
  src/lua-5.4.7/src/lapi.c
  src/lua-5.4.7/src/lstate.c
  src/lua-5.4.7/src/lopcodes.c
  src/lua-5.4.7/src/loadlib.c
  src/lua-5.4.7/src/lmathlib.c
  src/lua-5.4.7/src/liolib.c
  src/lua-5.4.7/src/luac.c
  src/lua-5.4.7/src/lfunc.c
  src/lua-5.4.7/src/ltablib.c
  src/lua-5.4.7/src/ldblib.c
  src/lua-5.4.7/src/lua.c
  src/lua-5.4.7/src/lbaselib.c
  src/lua-5.4.7/src/lutf8lib.c
  src/lua-5.4.7/src/lundump.c
  src/lua-5.4.7/src/ldump.c
  src/lua-5.4.7/src/lgc.c
  src/lua-5.4.7/src/lcode.c
  src/lua-5.4.7/src/ldo.c
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_libraries(${PROJECT_NAME}
  vita2d
  SceDisplay_stub
  SceAppMgr_stub
  SceGxm_stub
  SceSysmodule_stub
  SceCtrl_stub
  SceCommonDialog_stub
  freetype
  png
  bz2
  z
  m
  SceAppUtil_stub
  ScePower_stub
  SceShellSvc_stub
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE repatch_ex.skprx repatch_ex.skprx # https://github.com/SonicMastr/rePatch-reLoaded/releases/tag/v2.1
  FILE fagdec_vpk/Go-ahead-and-install_me.vpk fagdec_vpk/Go-ahead-and-install_me.vpk # https://github.com/TeamFAPS/PSVita-RE-tools/tree/178948c0866b6ad3246ab607886dd6b620628b86/FAGDec/build

  FILE patch.lua patch.lua
  FILE ps3_test_patch_file.elf ps3_test_patch_file.elf
  FILE vita_test_patch_file.velf vita_test_patch_file.velf
  )
