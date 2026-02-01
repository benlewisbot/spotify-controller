/**
 * @file lv_conf.h
 * Configuration file for LVGL 8.x
 * Optimized for ESP32 with PSRAM
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
   MEMORY SETTINGS
 *=========================*/

/* Size of the memory available for `lv_malloc()` in bytes (>= 2kB) */
#if defined(CONFIG_SPIRAM_USE_MALLOC) || defined(BOARD_HAS_PSRAM)
    /* Use PSRAM for LVGL */
    #define LV_MEM_SIZE (1024U * 1024U)  /* 1MB from PSRAM */
    #define LV_MEM_ADR 0                 /* 0: unused, else set address */
    #define LV_MEM_AUTO_DEFRAG 1         /* Automatically defrag memory */
#else
    /* Limited RAM mode */
    #define LV_MEM_SIZE (128U * 1024U)   /* 128KB from internal RAM */
    #define LV_MEM_ADR 0
    #define LV_MEM_AUTO_DEFRAG 1
#endif

/* Set an address for the memory pool instead of allocating it as a normal array */
#define LV_MEM_ADR 0

/* Use standard `memcpy` and `memset` instead of LVGL's own functions */
#define LV_MEMCPY_MEMSET_STD 1

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period in milliseconds */
#define LV_DEF_REFR_PERIOD 30

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/*========================
   OPERATING SYSTEM
 *========================*/

/* Select an operating system to use. Possible options:
 * - LV_OS_NONE
 * - LV_OS_PTHREAD
 * - LV_OS_FREERTOS
 * - LV_OS_CMSIS_RTOS2
 * - LV_OS_RTTHREAD
 * - LV_OS_WINDOWS
 * - LV_OS_CUSTOM
 */

#define LV_USE_OS LV_OS_NONE

/*====================
   FONT USAGE
 *====================*/

/* Montserrat fonts with various sizes */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Demonstrate special font features */
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/* Enable it if you have fonts with a lot of characters */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/* Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

/* Custom font declarations (define them here or in your own file) */
/* #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) */

/* Always set a default font */
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

/* Documentation of widgets: https://docs.lvgl.io/latest/en/html/widgets/index.html */

#define LV_WIDGETS_HAS_DEFAULT_VALUE  1

#define LV_USE_ANIMIMG    1
#define LV_USE_BAR        1
#define LV_USE_BUTTON     1
#define LV_USE_BUTTONMATRIX 1
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     1
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_DROPDOWN   1
#define LV_USE_IMAGE      1
#define LV_USE_IMAGEBUTTON 1
#define LV_USE_KEYBOARD   0
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
#define LV_USE_SPINNER    0
#define LV_USE_SWITCH     0
#define LV_USE_TEXTAREA   0
#define LV_USE_TABLE      0
#define LV_USE_TABVIEW    1
#define LV_USE_TILEVIEW   1
#define LV_USE_WIN        1

/*==================
   LAYOUTS
 *==================*/

/* A layout is used to arrange children in a parent object */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
   THEMES
 *====================*/

/* A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT 1

/* A very simple theme designed for small screens */
#define LV_USE_THEME_BASIC 1

/* A theme designed to look like a mobile OS */
#define LV_USE_THEME_MONO 1

/*==================
   LAYOUTS
 *==================*/

/* A layout is used to arrange children in a parent object */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
   OTHERS
 *====================*/

/*1: Enable custom mouse pointer on LVGL*/
#define LV_USE_MOUSE 0
#define LV_USE_POINTER 0

/*1: Enable file system*/
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_DRIVERS 1

/*1: Enable API to take screenshots for LVGL objects*/
#define LV_USE_SNAPSHOT 0

/*1: Enable system monitor component*/
#define LV_USE_STDLIB_MALLOC 1
#define LV_USE_STDLIB_STRING 1
#define LV_USE_STDLIB_SPRINTF 1
#define LV_USE_STDLIB_MATH 1
#define LV_USE_STDLIB_GTOD 0
#define LV_USE_STDLIB_QSORT 0

/*==================
   COMPONENTS
 *==================*/

#define LV_USE_ATC 0
#define LV_USE_FREETYPE 0
#define LV_USE_TINY_TTF 0
#define LV_USE_RLOTTIE 0
#define LV_USE_RTTYPED 0
#define LV_USE_LZ4 0
#define LV_USE_THORVG 0
#define LV_USE_PNG 1
#define LV_USE_SJPG 1
#define LV_USE_GIF 0
#define LV_USE_BMP 1
#define LV_USE_FFMPEG 0
#define LV_USE_QRCODE 0
#define LV_USE_FREETYPE 0
#define LV_USE_TINY_TTF 0

/*==================
   EXAMPLES
 *==================*/

/* Enable the examples to be built with the library */
#define LV_BUILD_EXAMPLES 0

/*================--
 * LVGL SETTINGS
 *=================*/

/* Max depth to call the stack. Usage: lv_task_handler(), lv_obj_handler(), etc.*/
#define LV_REFR_MAX_PAUSE 1

/* Enable flushing render on partial rendering areas */
#define LV_USE_REFR_AREA 1

/* Default alignment for a new object */
#define LV_OBJ_DEFAULT_ALIGN LV_ALIGN_TOP_LEFT

/* Default text alignment for a new object */
#define LV_OBJ_DEFAULT_TEXT_ALIGN LV_TEXT_ALIGN_AUTO

/* Default text color */
#define LV_COLOR_DEFAULT lv_color_black()

/* Default border color */
#define LV_COLOR_BORDER_DEFAULT lv_color_black()

/* Default background color */
#define LV_COLOR_SCR_DEFAULT lv_color_black()

/* Default circle color */
#define LV_COLOR_CIRCLE_DEFAULT lv_color_black()

/*==================
   PERFORMANCE
 *==================*/

/*1: Use a custom tick source. IMPORTANT: Must be enabled for FreeRTOS */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM == 1
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*Default Dot Per Inch. Used to initialize default sizes. E.g. font size, icon size*/
#define LV_DPI_DEF 130

/*=========================
   OPERATING SYSTEM
 *=========================*/

/*Select an operating system to use. Possible options:
 * - LV_OS_NONE
 * - LV_OS_PTHREAD
 * - LV_OS_FREERTOS
 * - LV_OS_CMSIS_RTOS2
 * - LV_OS_RTTHREAD
 * - LV_OS_WINDOWS
 * - LV_OS_CUSTOM
*/

#define LV_USE_OS LV_OS_NONE

/*==================
   OTHERS
 *==================*/

/*Default object refresh period. Higher value means lower CPU usage and slower UI reaction*/
#define LV_DEF_REFR_PERIOD 30

/*Input device read period in milliseconds*/
#define LV_INDEV_DEF_READ_PERIOD 30

/*Default Drag throw slow-down in [%]. Greater value -> faster slow-down*/
#define LV_DRAG_THROW_MIN_SPEED 20

/*Use the "dot" operations on colors without 'lv_color_' prefix (in headers)*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)

/*==================
   LOG SETTINGS
 *==================*/

/*Enable the log module*/
#define LV_USE_LOG 1
#if LV_USE_LOG

  /*How important log should be added:*/
  #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

  /*1: Print the log with 'printf', 0: User need to register a callback*/
  #define LV_LOG_PRINTF 1

  /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
  #define LV_LOG_TRACE_MEM        1
  #define LV_LOG_TRACE_TIMER      1
  #define LV_LOG_TRACE_INDEV      1
  #define LV_LOG_TRACE_DISP_REFR  1
  #define LV_LOG_TRACE_EVENT      1
  #define LV_LOG_TRACE_OBJ_CREATE 1
  #define LV_LOG_TRACE_LAYOUT     1
  #define LV_LOG_TRACE_ANIM       1

#endif  /*LV_USE_LOG*/

/*==================
   ASSERTS
 *==================*/

/*Enable asserts if an operation fails or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
#define LV_USE_ASSERT_NULL          1   /*Check if the parameter is NULL. (Very fast, recommended)*/
#define LV_USE_ASSERT_MALLOC        1   /*Checks is the memory is successfully allocated or no. (Very fast, recommended)*/
#define LV_USE_ASSERT_STYLE         0   /*Check if the styles are properly initialized. (Very fast, but checks all style flags)*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /*Check if the memory is successfully allocated or no. (Very fast, recommended)*/

/*Add a custom handler on assert failure*/
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /*Halt by default*/

/*==================
   OTHERS
 *==================*/

/*1: Show CPU usage and FPS count*/
#define LV_USE_PERF_MONITOR 0

/*1: Show the used memory and the memory fragmentation*/
#define LV_USE_MEM_MONITOR 0

/*1: Draw random colored rectangles on the invalidated (redrawn) areas*/
#define LV_USE_REFR_DEBUG 0

/*Change the built-in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE <stdio.h>
    #define lv_snprintf  snprintf
    #define lv_vsnprintf vsnprintf
#endif  /*LV_SPRINTF_CUSTOM*/

/*Set LV_FS_DEFAULT_* macros */
#define LV_FS_DEFAULT_PATH ""
#define LV_FS_DEFAULT_WRITABLE 0

/*====================
   IMAGE SETTINGS
 *====================*/

/*1: Enable built-in image decoder (PNG, BMP, JPG) */
#define LV_USE_PNG 1
#define LV_USE_BMP 1
#define LV_USE_SJPG 1

/*Set the maximal image width and height. Larger images will be scaled.*/
#define LV_IMG_CACHE_DEF_SIZE 0

/*====================
   API SETTINGS
 *====================*/

/*Enable or disable API functions */
#define LV_API_ENABLE_V7_COMPAT 0

/*==================
   GZIP SUPPORT
 *==================*/

#define LV_USE_GZ 0

#endif /*LV_CONF_H*/
