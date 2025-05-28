
#pragma once

// UIDefines.h
#include "QSPI.h"
extern QFLASH_SECTION DadQSPI::cQSPI_FlasherStorage  __FlashStorage;

// ================================================================================================================================
// SCREEN Parameter
//
// 	|------------------------ SCREEN_WIDTH -----------------------|
// 	|------------------------- MAIN_WIDTH ------------------------|
// 	|----|                    SCREEN_EDGE                    |----|
// 	     |------------|     MENU_ITEM_WIDTH
//  |-------------------|     PARAM_WIDTH
//   _____________________________________________________________    _                     _                     _
// 	|    |            |            |            |            |    |    | MENU_HEIGHT         |                     | SCREEN_HEIGHT
// 	|____|____________|____________|____________|____________|____|   _|                    _|                     |
// 	|                   |                     |                   |    | PARAM_NAME_HEIGHT   |                     |
// 	|___________________|_____________________|___________________|   _|                    _|                     |
// 	|                   |                     |                   |    | PARAM_FORM_HEIGHT   |                     |
// 	|                   |                     |                   |    |                     |                     |
// 	|                   |                     |                   |    |                     |                     |
// 	|                   |                     |                   |    |                     |                     |
// 	|                   |                     |                   |    |                     |                     |
// 	|___________________|____________________ |___________________|   _|                     |                     |
//	|                   |                     |                   |    | PARAM_VAL_HEIGHT    |                     |
//	|___________________|_____________________|___________________|   _|                    _|                     |
//	|                                                             |                          | MAIN_HEIGHT         |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|                                                             |                          |                     |
//	|_____________________________________________________________|                         _|                    _|


#define SCREEN_WIDTH 320
#define SCREEN_EDGE 10
#define NB_MENU_ITEM 4
#define NB_PARAM_ITEM 3
#define MAIN_WIDTH SCREEN_WIDTH
#define MENU_ITEM_WIDTH (MAIN_WIDTH - SCREEN_EDGE - SCREEN_EDGE) / NB_MENU_ITEM
#define PARAM_WIDTH (MAIN_WIDTH / NB_PARAM_ITEM)

#define SCREEN_HEIGHT 240
#define MENU_HEIGHT 22
#define PARAM_NAME_HEIGHT 28
#define PARAM_FORM_HEIGHT 72
#define PARAM_VAL_HEIGHT  28
#define PARAM_HEIGHT (PARAM_NAME_HEIGHT+PARAM_FORM_HEIGHT+PARAM_VAL_HEIGHT)
#define MAIN_HEIGHT (SCREEN_HEIGHT - (MENU_HEIGHT + PARAM_HEIGHT))

// ================================================================================================================================
// Font definitions, retrieving font data from QSPI storage
#ifdef FONTH
#pragma GCC push_options
#pragma GCC optimize ("O0")
#include "Font_11p.h"
#include "Font_12p.h"
#include "Font_22p.h"
#include "Font_32p.h"
#include "Font_20pb.h"
#include "Font_36pb.h"
#include "Font_48pb.h"
#include "Penda.h"
#pragma GCC pop_options
#define FONTS 	&__Font_11p
#define FONTM 	&__Font_12p
#define FONTL 	&__Font_22p
#define FONTXL	&__Font_32p
#define FONTLB	&__Font_20pb
#define FONTXLB	&__Font_36pb
#define FONTXXL	&__Font_48pb
#define PENDA   Penda_map

#else
#define FONTS (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_11p.bin")
#define FONTM (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_12p.bin")
#define FONTL (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_22p.bin")
#define FONTXL (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_32p.bin")
#define FONTLB (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_20pb.bin")
#define FONTXLB (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_36pb.bin")
#define FONTXXL (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Font_48pb.bin")
#define PENDA __FlashStorage.GetFilePtr("Penda.png")
#endif

// ================================================================================================================================
// Colors
#define BACKCOLOR DadGFX::sColor(25,25,10,255)
#define LAYER_PARAMETER_NAME_COLOR DadGFX::sColor(255,255,255,255)

#define LAYER_PARAMETER_VALUE_COLOR DadGFX::sColor(255,255,255,255)

#define LAYER_PARAMETER_MAIN_COLOR DadGFX::sColor(255,255,255,255)
#define LAYER_POT_COLOR DadGFX::sColor(255,255,255,255)

//#define LAYER_POT_INDEX_COLOR DadGFX::sColor(255, 50, 221, 255)
#define LAYER_POT_INDEX_COLOR DadGFX::sColor(94, 235, 201, 255)
#define LAYER_POT_LINE_COLOR DadGFX::sColor(78, 191, 164, 255)
//#define LAYER_POT_LINE_COLOR DadGFX::sColor(83, 252, 212, 255)
//#define LAYER_POT_LINE_COLOR DadGFX::sColor(171, 255, 236, 255)

//#define LAYER_POT_INDEX_COLOR DadGFX::sColor(50, 238, 255, 255)
//#define LAYER_POT_INDEX_COLOR DadGFX::sColor(255,100,50,255)

//#define MENU_ACTIVE_ITEM_COLOR DadGFX::sColor(67, 168, 164, 255)
#define MENU_ACTIVE_ITEM_COLOR DadGFX::sColor(67, 168, 144, 255)
//#define MENU_ACTIVE_ITEM_COLOR DadGFX::sColor(110,110,120,255)
#define MENU_SELETED_ITEM_COLOR DadGFX::sColor(255, 255, 255, 255)
#define MENU_UNSELETED_ITEM_COLOR DadGFX::sColor(255, 255, 255, 255)

//#define MENU_BACK_COLOR  DadGFX::sColor(45,45,30,255)
#define MENU_BACK_COLOR  DadGFX::sColor(71, 99, 92, 255)
//#define MAIN_BACK_COLOR  DadGFX::sColor(60,60,75,255)
#define MAIN_BACK_COLOR  DadGFX::sColor(114, 163, 151, 255)


//#define SPLASHSCREEN_BACK_COLOR  DadGFX::sColor(180, 224, 213, 255)
//#define SPLASHSCREEN_TEXT_COLOR  DadGFX::sColor(0, 0, 0, 255)
#define SPLASHSCREEN_BACK_COLOR  DadGFX::sColor(114, 163, 151, 255)
#define SPLASHSCREEN_TEXT_COLOR  DadGFX::sColor(180, 224, 213, 255)

// ================================================================================================================================
// Form parameter

// Parameters for potentiometer graphical representation
#define PARAM_POT_RADIUS 28
#define PARAM_POT_ALPHA_MIN 30
#define PARAM_POT_ALPHA_MAX 360 - PARAM_POT_ALPHA_MIN
#define PARAM_POT_ALPHA PARAM_POT_ALPHA_MAX - PARAM_POT_ALPHA_MIN

// Parameters for potentiometer discret graphical representation
#define PARAM_DISCRET_RADIUS 5
#define PARAM_DISCRET_POT_RADIUS 26
