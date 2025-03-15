#ifndef COLOURS_CONFIG_H_   /* Include guard */
#define COLOURS_CONFIG_H_

#include "types_for_my_patcher_app.h"

extern u32 TITLE_FONT_COLOUR;
extern u32 TITLE_BG_COLOUR;
extern u32 SELECTABLE_NORMAL_FONT_COLOUR;
extern u32 TURNED_ON_FONT_COLOUR;
extern u32 BACKGROUND_COLOUR;
extern u32 SELECTED_FONT_BG_COLOUR;
extern u32 UNSELECTED_FONT_BG_COLOUR;
extern u32 ERROR_MESSAGE_COLOUR;
extern u32 ERROR_MESSAGE_BG_COLOUR;
extern u32 SUCCESS_MESSAGE_COLOUR;
extern u32 SUCCESS_MESSAGE_BG_COLOUR;

void load_config();

#endif // COLOURS_CONFIG_H_