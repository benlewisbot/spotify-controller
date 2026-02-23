/**
 * @file lv_conf.h
 * Configuration file for LVGL 9.x
 * Optimized for ESP32-S3 with PSRAM (Guition ESP32-S3-4848S040)
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB
 *=========================*/

/* Use C standard library malloc/free - on ESP32-S3 with PSRAM this allocates
   from PSRAM for large blocks, avoiding the 1MB static DRAM array problem */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/* MEM_SIZE is not used with LV_STDLIB_CLIB, but kept for reference */
#define LV_MEM_SIZE (512U * 1024U)

/*========================
   OPERATING SYSTEM
 *========================*/

#define LV_USE_OS LV_OS_NONE

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period in milliseconds */
#define LV_DEF_REFR_PERIOD 30

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Default Dot Per Inch. Used to initialize default sizes */
#define LV_DPI_DEF 130

/*==================
   PERFORMANCE
 *==================*/

/* LVGL 9 uses lv_tick_set_cb() instead of LV_TICK_CUSTOM.
   The tick callback is set in DisplayManager::initLVGL(). */

/*====================
   FONT USAGE
 *====================*/

/* Montserrat fonts with various sizes */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

/* Special font features */
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/* Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

/* Default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/* Enable handling large number of fonts */
#define LV_FONT_FMT_TXT_LARGE 0

/* Enables/disables support for compressed fonts */
#define LV_USE_FONT_COMPRESSED 0

/* Enable subpixel rendering */
#define LV_FONT_SUBPX_BGR 0

/*===================
   WIDGET USAGE
 *===================*/

#define LV_WIDGETS_HAS_DEFAULT_VALUE  1

#define LV_USE_ANIMIMG    1
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BUTTON     1
#define LV_USE_BUTTONMATRIX 1
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     1
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMAGE      1
#define LV_USE_IMAGEBUTTON 1
#define LV_USE_KEYBOARD   1
#define LV_USE_LABEL      1
#define LV_USE_LED        0
#define LV_USE_LINE       1
#define LV_USE_LIST       1
#define LV_USE_MENU       0
#define LV_USE_MSGBOX     1
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      0
#define LV_USE_SLIDER     1
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    1
#define LV_USE_SWITCH     1
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      0
#define LV_USE_TABVIEW    1
#define LV_USE_TILEVIEW   1
#define LV_USE_WIN        1

/*==================
   LAYOUTS
 *==================*/

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
   THEMES
 *====================*/

#define LV_USE_THEME_DEFAULT 1
#define LV_USE_THEME_BASIC 1
#define LV_USE_THEME_MONO 0

/*====================
   OTHERS
 *====================*/

/* File system */
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0

/* Screenshots */
#define LV_USE_SNAPSHOT 0

/*==================
   COMPONENTS
 *==================*/

#define LV_USE_FREETYPE 0
#define LV_USE_TINY_TTF 0
#define LV_USE_RLOTTIE 0
#define LV_USE_LZ4 0
#define LV_USE_THORVG 0
#define LV_USE_PNG 1
#define LV_USE_SJPG 1
#define LV_USE_GIF 0
#define LV_USE_BMP 1
#define LV_USE_FFMPEG 0
#define LV_USE_QRCODE 0

/*==================
   EXAMPLES
 *==================*/

#define LV_BUILD_EXAMPLES 0

/*==================
   LOG SETTINGS
 *==================*/

#define LV_USE_LOG 1
#if LV_USE_LOG

  #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

  #define LV_LOG_PRINTF 1

  #define LV_LOG_TRACE_MEM        0
  #define LV_LOG_TRACE_TIMER      0
  #define LV_LOG_TRACE_INDEV      0
  #define LV_LOG_TRACE_DISP_REFR  0
  #define LV_LOG_TRACE_EVENT      0
  #define LV_LOG_TRACE_OBJ_CREATE 0
  #define LV_LOG_TRACE_LAYOUT     0
  #define LV_LOG_TRACE_ANIM       0

#endif  /*LV_USE_LOG*/

/*==================
   ASSERTS
 *==================*/

#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0

#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);

/*==================
   DEBUG / MONITOR
 *==================*/

#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0
#define LV_USE_REFR_DEBUG 0

#endif /*LV_CONF_H*/
