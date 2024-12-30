/**
 * @file lv_conf.h
 * 配置文件 v8.3.11
 */

/*
 * 将此文件复制为 `lv_conf.h`
 * 1. 直接放在 `lvgl` 文件夹旁边
 * 2. 或者放在其他地方并
 *    - 定义 `LV_CONF_INCLUDE_SIMPLE`
 *    - 添加路径作为包含路径
 */

/* clang-format off */
#if 1 /*设置为 "1" 以启用内容*/

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
#pragma GCC diagnostic ignored "-Wliteral-conversion"

/*====================
   COLOR SETTINGS
 *====================*/

/*颜色深度: 1 (每像素1字节), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*交换 RGB565 颜色的2个字节。适用于显示器具有8位接口（如SPI）的情况*/
#define LV_COLOR_16_SWAP 1

/*启用绘制透明背景的功能。
 *如果使用 opa 和 transform_* 样式属性，则需要此功能。
 *也可以在另一个图层之上使用，例如OSD菜单或视频播放器。*/
#define LV_COLOR_SCREEN_TRANSP 1

/*调整颜色混合函数的舍入方式。GPU可能以不同的方式计算颜色混合。
 *0: 向下舍入, 64: 从 x.75 向上舍入, 128: 从一半向上舍入, 192: 从 x.25 向上舍入, 254: 向上舍入 */
#define LV_COLOR_MIX_ROUND_OFS 0

/*图像中具有此颜色的像素在色度键控时将不会被绘制*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*纯绿色*/

/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: 使用自定义的 malloc/free, 0: 使用内置的 `lv_mem_alloc()` 和 `lv_mem_free()`*/
#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM == 0
    /*`lv_mem_alloc()` 可用的内存大小 (>= 2kB)*/
    #define LV_MEM_SIZE (128U * 1024U)          /*[字节]*/
    /*为内存池设置一个地址，而不是将其作为普通数组分配。也可以在外部SRAM中使用。*/
    /*如果不使用地址，而是使用内存分配器，则调用该分配器为LVGL获取内存池。例如 my_malloc*/
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC 
    #endif

#else       /*LV_MEM_CUSTOM*/
    #define LV_MEM_CUSTOM_INCLUDE "bsp.h"   /*动态内存函数的头文件*/
    #define LV_MEM_CUSTOM_ALLOC   os_malloc
    #define LV_MEM_CUSTOM_FREE    os_free
    #define LV_MEM_CUSTOM_REALLOC os_realloc
#endif     /*LV_MEM_CUSTOM*/

/*用于渲染和其他内部处理机制的中间内存缓冲区的数量。
 *如果缓冲区不足，将看到错误日志消息。*/
#define LV_MEM_BUF_MAX_NUM 16

/*使用标准的 `memcpy` 和 `memset` 而不是 LVGL 自己的函数。（可能会更快或更慢）*/
#define LV_MEMCPY_MEMSET_STD 0

/*====================
   HAL SETTINGS
 *====================*/

/*默认显示刷新周期。LVGL 将在此周期时间内重绘更改的区域*/
#define LV_DISP_DEF_REFR_PERIOD 33      /*[ms]*/

/*输入设备读取周期（以毫秒为单位）*/
#define LV_INDEV_DEF_READ_PERIOD 20     /*[ms]*/

/*使用自定义的 tick 源，以毫秒为单位报告经过的时间。
 *它消除了手动更新 tick 的需要，使用 `lv_tick_inc()`)*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE   "bsp.h"   /*系统时间函数的头文件*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (TIMESTAMP_US_GET()/1000LL)    /*计算当前系统时间的表达式，单位为 ms*/
    /*如果使用 ESP32 组件*/
    // #define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"
    // #define LV_TICK_CUSTOM_SYS_TIME_EXPR ((esp_timer_get_time() / 1000LL))
#endif   /*LV_TICK_CUSTOM*/

/*默认每英寸点数。用于初始化默认大小，如小部件大小、样式填充。
 *(不太重要，可以根据需要调整以修改默认大小和间距)*/
#define LV_DPI_DEF 130     /*[px/英寸]*/

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Drawing
 *-----------*/

/*启用复杂的绘图引擎。
 *需要绘制阴影、渐变、圆角、圆形、弧形、倾斜线条、图像变换或任何遮罩时需要此功能*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

    /*允许缓存一些阴影计算。
    *LV_SHADOW_CACHE_SIZE 是最大缓存的阴影大小，其中阴影大小为 `shadow_width + radius`
    *缓存的成本为 LV_SHADOW_CACHE_SIZE^2 的 RAM*/
    #define LV_SHADOW_CACHE_SIZE 2

    /*设置最大缓存的圆形数据数量。
    *1/4 圆的周长被保存用于抗锯齿
    *每个圆使用 radius * 4 字节的内存（最常用的半径被保存）
    *0: 禁用缓存 */
    #define LV_CIRCLE_CACHE_SIZE 4
#endif /*LV_DRAW_COMPLEX*/

/**
 * "简单图层" 用于当小部件的 `style_opa < 255` 时，将小部件缓冲到图层中
 * 并以给定的不透明度作为图像进行混合。
 * 注意 `bg_opa`, `text_opa` 等不需要缓冲到图层中)
 * 小部件可以缓冲到较小的块中，以避免使用大缓冲区。
 *
 * - LV_LAYER_SIMPLE_BUF_SIZE: [字节] 最佳目标缓冲区大小。LVGL 将尝试分配它
 * - LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE: [字节] 如果 `LV_LAYER_SIMPLE_BUF_SIZE` 无法分配，则使用此大小。
 *
 * 两个缓冲区大小均为字节。
 * "变换图层"（使用 transform_angle/zoom 属性）使用更大的缓冲区
 * 并且不能分块绘制。因此这些设置仅影响具有不透明度的小部件。
 */
#define LV_LAYER_SIMPLE_BUF_SIZE          (24 * 1024)
#define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE (3 * 1024)

/*默认图像缓存大小。图像缓存保持图像打开。
 *如果仅使用内置的图像格式，则缓存没有实际优势。（即如果没有添加新的图像解码器）
 *使用复杂的图像解码器（如PNG或JPG）时，缓存可以节省连续打开/解码图像的时间。
 *然而，打开的图像可能会消耗额外的RAM。
 *0: 禁用缓存*/
#define LV_IMG_CACHE_DEF_SIZE 0

/*每个渐变允许的最大停止点数量。增加此值以允许更多停止点。
 *每个额外的停止点增加 (sizeof(lv_color_t) + 1) 字节的内存*/
#define LV_GRADIENT_MAX_STOPS 2

/*默认渐变缓冲区大小。
 *当 LVGL 计算渐变 "地图" 时，可以将其缓存以避免再次计算。
 *LV_GRAD_CACHE_DEF_SIZE 设置此缓存的大小（以字节为单位）。
 *如果缓存太小，地图将仅在绘图时分配。
 *0 表示不缓存。*/
#define LV_GRAD_CACHE_DEF_SIZE 512

/*允许对渐变进行抖动（以在有限颜色深度的显示器上实现视觉平滑的颜色渐变）
 *LV_DITHER_GRADIENT 意味着为对象的渲染表面分配一个或两个额外的行
 *内存消耗增加为 (32 位 * 对象宽度) 加上 24 位 * 对象宽度（如果使用误差扩散） */
#define LV_DITHER_GRADIENT 0
#if LV_DITHER_GRADIENT
    /*添加对误差扩散抖动的支持。
     *误差扩散抖动可以获得更好的视觉效果，但在绘图时会消耗更多的CPU和内存。
     *内存消耗增加为 (24 位 * 对象宽度)*/
    #define LV_DITHER_ERROR_DIFFUSION 0
#endif

/*用于旋转的最大缓冲区大小。
 *仅在显示驱动程序中启用软件旋转时使用。*/
#define LV_DISP_ROT_MAX_BUF (10*1024)

/*-------------
 * GPU
 *-----------*/

/*使用 Arm 的 2D 加速库 Arm-2D */
#define LV_USE_GPU_ARM2D 0

/*使用 STM32 的 DMA2D (即 Chrom Art) GPU*/
#define LV_USE_GPU_STM32_DMA2D 0
#if LV_USE_GPU_STM32_DMA2D
    /*必须定义以包含目标处理器的 CMSIS 头文件
    e.g. "stm32f7xx.h" 或 "stm32f4xx.h"*/
    #define LV_GPU_DMA2D_CMSIS_INCLUDE  "stm32h7xx.h"
#endif

/*启用 RA6M3 G2D GPU*/
#define LV_USE_GPU_RA6M3_G2D 0
#if LV_USE_GPU_RA6M3_G2D
    /*目标处理器的包含路径
    e.g. "hal_data.h"*/
    #define LV_GPU_RA6M3_G2D_INCLUDE "hal_data.h"
#endif

/*使用 SWM341 的 DMA2D GPU*/
#define LV_USE_GPU_SWM341_DMA2D 0
#if LV_USE_GPU_SWM341_DMA2D
    #define LV_GPU_SWM341_DMA2D_INCLUDE "SWM341.h"
#endif

/*使用 NXP 的 PXP GPU iMX RTxxx 平台*/
#define LV_USE_GPU_NXP_PXP 0
#if LV_USE_GPU_NXP_PXP
    /*1: 添加 PXP 的默认裸机和 FreeRTOS 中断处理例程（lv_gpu_nxp_pxp_osa.c）
    *   并在 lv_init() 期间自动调用 lv_gpu_nxp_pxp_init()。注意符号 SDK_OS_FREE_RTOS
    *   必须定义以使用 FreeRTOS OSA，否则选择裸机实现。
    *0: 在 lv_init() 之前手动调用 lv_gpu_nxp_pxp_init()
    */
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

/*使用 NXP 的 VG-Lite GPU iMX RTxxx 平台*/
#define LV_USE_GPU_NXP_VG_LITE 0

/*使用 SDL 渲染器 API*/
#define LV_USE_GPU_SDL 0
#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    /*纹理缓存大小，默认 8MB*/
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
    /*自定义混合模式用于遮罩绘制，如果需要与较旧的 SDL2 库链接，请禁用*/
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * Logging
 *-----------*/

/*启用日志模块*/
#define LV_USE_LOG 1
#if LV_USE_LOG

    /*应添加日志的重要性级别:
    *LV_LOG_LEVEL_TRACE       大量日志以提供详细信息
    *LV_LOG_LEVEL_INFO        记录重要事件
    *LV_LOG_LEVEL_WARN        记录不希望发生但未引起问题的事件
    *LV_LOG_LEVEL_ERROR       仅记录系统可能失败的严重问题
    *LV_LOG_LEVEL_USER        仅记录用户添加的日志
    *LV_LOG_LEVEL_NONE        不记录任何内容*/
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /*1: 使用 'printf' 打印日志；
    *0: 用户需要使用 `lv_log_register_print_cb()` 注册回调*/
    #define LV_LOG_PRINTF 0

    /*启用/禁用模块中产生大量日志的 LV_LOG_TRACE*/
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1

#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts
 *-----------*/

/*如果操作失败或发现无效数据，启用断言。
 *如果启用了 LV_USE_LOG，失败时将打印错误消息*/
#define LV_USE_ASSERT_NULL          1   /*检查参数是否为 NULL。（非常快，推荐）*/
#define LV_USE_ASSERT_MALLOC        1   /*检查内存是否成功分配。（非常快，推荐）*/
#define LV_USE_ASSERT_STYLE         1   /*检查样式是否正确初始化。（非常快，推荐）*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /*在关键操作后检查 `lv_mem` 的完整性。（慢）*/
#define LV_USE_ASSERT_OBJ           0   /*检查对象的类型和存在性（例如未删除）。（慢）*/

/*断言发生时添加自定义处理程序，例如重启 MCU*/
#define LV_ASSERT_HANDLER_INCLUDE <stdio.h>
#define LV_ASSERT_HANDLER while(1);

/*-------------
 * Others
 *-----------*/

/*1: 显示 CPU 使用率和 FPS 计数*/
#define LV_USE_PERF_MONITOR 0
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_OUT_RIGHT_BOTTOM
#endif

/*1: 显示已用内存和内存碎片
 * 需要 LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR 0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/*1: 在重绘区域上绘制随机颜色的矩形*/
#define LV_USE_REFR_DEBUG 0

/*更改内置的 (v)snprintf 函数*/
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE <stdio.h>
    #define lv_snprintf  snprintf
    #define lv_vsnprintf vsnprintf
#else   /*LV_SPRINTF_CUSTOM*/
    #define LV_SPRINTF_USE_FLOAT 0
#endif  /*LV_SPRINTF_CUSTOM*/

#define LV_USE_USER_DATA 1

/*垃圾收集器设置
 *用于将 lvgl 绑定到更高级别的语言，并且内存由该语言管理*/
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"                           /*包含垃圾收集器相关内容*/
#endif /*LV_ENABLE_GC*/

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/*对于大端系统设置为 1*/
#define LV_BIG_ENDIAN_SYSTEM 0

/*为 `lv_tick_inc` 函数定义自定义属性*/
#define LV_ATTRIBUTE_TICK_INC

/*为 `lv_timer_handler` 函数定义自定义属性*/
#define LV_ATTRIBUTE_TIMER_HANDLER

/*为 `lv_disp_flush_ready` 函数定义自定义属性*/
#define LV_ATTRIBUTE_FLUSH_READY

/*缓冲区所需的内存对齐大小*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4

/*将在需要对齐内存的地方添加（例如，使用 -Os 数据可能默认不对齐到边界）。
 *例如 __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN __attribute__((aligned(4)))

/*标记大常量数组的属性，例如字体位图*/
#define LV_ATTRIBUTE_LARGE_CONST 

/*在 RAM 中声明大数组的编译器前缀*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY  

/*将性能关键函数放入更快的内存（例如 RAM）*/
#define LV_ATTRIBUTE_FAST_MEM

/*前缀用于在 GPU 加速操作中使用的变量，这些变量通常需要放置在 DMA 可访问的 RAM 段中*/
#define LV_ATTRIBUTE_DMA 

/*将整数常量导出到绑定。此宏用于常量，格式为 LV_<CONST>，
 *这些常量也应出现在 LVGL 绑定 API 中，例如 Micropython。*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*默认值仅防止 GCC 警告*/

/*将默认 -32k..32k 坐标范围扩展到 -4M..4M，使用 int32_t 代替 int16_t*/
#define LV_USE_LARGE_COORD 0

/*==================
 *   FONT USAGE
 *===================*/

/*使用 ASCII 范围和一些符号的 Montserrat 字体，bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 0
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
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

/*演示特殊功能*/
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*希伯来语、阿拉伯语、波斯语字母及其所有形式*/
#define LV_FONT_SIMSUN_16_CJK            0  /*1000 个最常见的 CJK 部首*/

/*像素完美的等宽字体*/
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/*可选在此处声明自定义字体。
 *您可以将这些字体用作默认字体，并且它们将在全局范围内可用。
 *例如 #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE

/*始终设置默认字体*/
#define LV_FONT_DEFAULT &lv_font_montserrat_12

/*启用处理大字体和/或包含大量字符的字体。
 *限制取决于字体大小、字体面和 bpp。
 *如果字体需要此功能，将触发编译器错误。*/
#define LV_FONT_FMT_TXT_LARGE 0

/*启用压缩字体支持。*/
#define LV_USE_FONT_COMPRESSED 0

/*启用子像素渲染*/
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    /*设置显示器的像素顺序。物理 RGB 通道的顺序。对于 "正常" 字体无关紧要。*/
    #define LV_FONT_SUBPX_BGR 0  /*0: RGB; 1: BGR 顺序*/
#endif

/*启用在找不到字形描述时绘制占位符*/
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 *  TEXT SETTINGS
 *=================*/

/**
 * 为字符串选择字符编码。
 * 您的 IDE 或编辑器应具有相同的字符编码
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/*可以在这些字符上断行*/
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*如果一个单词至少这么长，将在 "最漂亮" 的地方断行
 *要禁用，设置为 <= 0*/
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/*长单词中断前放在一行上的最小字符数。
 *取决于 LV_TXT_LINE_BREAK_LONG_LEN。*/
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/*长单词中断后放在一行上的最小字符数。
 *取决于 LV_TXT_LINE_BREAK_LONG_LEN。*/
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*用于标记文本重新着色的控制字符。*/
#define LV_TXT_COLOR_CMD "#"

/*支持双向文本。允许混合从左到右和从右到左的文本。
 *方向将根据 Unicode 双向算法处理：
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    /*设置默认方向。支持的值：
    *`LV_BASE_DIR_LTR` 从左到右
    *`LV_BASE_DIR_RTL` 从右到左
    *`LV_BASE_DIR_AUTO` 检测文本的基本方向*/
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/*启用阿拉伯语/波斯语处理
 *在这些语言中，字符应根据其在文本中的位置替换为其他形式*/
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  WIDGET USAGE
 *================*/

/*小部件文档：https://docs.lvgl.io/latest/en/html/widgets/index.html*/

#define LV_USE_ARC        1

#define LV_USE_BAR        1

#define LV_USE_BTN        1

#define LV_USE_BTNMATRIX  1

#define LV_USE_CANVAS     1

#define LV_USE_CHECKBOX   1

#define LV_USE_DROPDOWN   1   /*需要：lv_label*/

#define LV_USE_IMG        1   /*需要：lv_label*/

#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1 /*启用选择标签文本*/
    #define LV_LABEL_LONG_TXT_HINT 1  /*存储一些额外信息以加速绘制非常长的文本*/
#endif

#define LV_USE_LINE       1

#define LV_USE_ROLLER     1   /*需要：lv_label*/
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES 7 /*无限滚轮时的额外 "页面" 数量*/
#endif

#define LV_USE_SLIDER     1   /*需要：lv_bar*/

#define LV_USE_SWITCH     1

#define LV_USE_TEXTAREA   1   /*需要：lv_label*/
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /*ms*/
#endif

#define LV_USE_TABLE      1

/*==================
 * EXTRA COMPONENTS
 *==================*/

/*-----------
 * Widgets
 *----------*/

#define LV_USE_ANALOGCLOCK 0

#define LV_USE_ANIMIMG 1

#define LV_USE_CALENDAR 1
#if LV_USE_CALENDAR
#define LV_CALENDAR_WEEK_STARTS_MONDAY 0
#if LV_CALENDAR_WEEK_STARTS_MONDAY == 0
#define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
#endif    /* LV_CALENDAR_WEEK_STARTS_MONDAY */
#define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
#define LV_USE_CALENDAR_HEADER_ARROW 1
#define LV_USE_CALENDAR_HEADER_DROPDOWN 1
#endif    /* LV_USE_CALENDAR */

#define LV_USE_CAROUSEL 1

#define LV_USE_CHART 1

#define LV_USE_COLORWHEEL 1

#define LV_USE_DCLOCK 1

#define LV_USE_IMGBTN 1

#define LV_USE_KEYBOARD 0

#define LV_USE_LED 1

#define LV_USE_LIST 1

#define LV_USE_MENU 1

#define LV_USE_METER 1

#define LV_USE_MSGBOX 1

#define LV_USE_RADIOBTN 1

#define LV_USE_SPAN 1
#if LV_USE_SPAN
/*A line text can contain maximum num of span descriptor */
#define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif    /* LV_USE_SPAN */

#define LV_USE_SPINBOX 1

#define LV_USE_SPINNER 1

#define LV_USE_TABVIEW 1

#define LV_USE_TILEVIEW 1

#define LV_USE_VIDEO 0

#define LV_USE_WIN 1

#define LV_USE_ZH_KEYBOARD 0
#if LV_USE_ZH_KEYBOARD
#define LV_ZH_KEYBOARD_MINI 0
#endif    /* LV_USE_ZH_KEYBOARD */

/*-----------
 * Themes
 *----------*/

/*一个简单、令人印象深刻且非常完整的主题*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT

    /*0: 浅色模式；1: 深色模式*/
    #define LV_THEME_DEFAULT_DARK 1

    /*1: 启用按下时增长*/
    #define LV_THEME_DEFAULT_GROW 1

    /*默认过渡时间 [ms]*/
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif /*LV_USE_THEME_DEFAULT*/

/*一个非常简单的主题，是自定义主题的良好起点*/
#define LV_USE_THEME_BASIC 1

/* 简洁主题*/
#define LV_USE_THEME_MONO 1

/*-----------
 * Layouts
 *----------*/

/*类似于 CSS 中 Flexbox 的布局*/
#define LV_USE_FLEX 1

/*类似于 CSS 中 Grid 的布局*/
#define LV_USE_GRID 1

/*---------------------
 * 3rd party libraries
 *--------------------*/

/*常见 API 的文件系统接口 */

/*API for fopen, fread, etc*/
#define LV_USE_FS_STDIO 0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'     /*设置驱动器可访问的上层字母（例如 'A'）*/
    #define LV_FS_STDIO_PATH ""         /*设置工作目录。文件/目录路径将附加到其后。*/
    #define LV_FS_STDIO_CACHE_SIZE 0    /*>0 以在 lv_fs_read() 中缓存此数量的字节*/
#endif

/*API for open, read, etc*/
#define LV_USE_FS_POSIX 0
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER '\0'     /*设置驱动器可访问的上层字母（例如 'A'）*/
    #define LV_FS_POSIX_PATH ""         /*设置工作目录。文件/目录路径将附加到其后。*/
    #define LV_FS_POSIX_CACHE_SIZE 0    /*>0 以在 lv_fs_read() 中缓存此数量的字节*/
#endif

/*API for CreateFile, ReadFile, etc*/
#define LV_USE_FS_WIN32 0
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER '\0'     /*设置驱动器可访问的上层字母（例如 'A'）*/
    #define LV_FS_WIN32_PATH ""         /*设置工作目录。文件/目录路径将附加到其后。*/
    #define LV_FS_WIN32_CACHE_SIZE 0    /*>0 以在 lv_fs_read() 中缓存此数量的字节*/
#endif

/*API for FATFS (需要单独添加)。使用 f_open, f_read, etc*/
#define LV_USE_FS_FATFS 0
#if LV_USE_FS_FATFS
    #define LV_FS_FATFS_LETTER '\0'     /*设置驱动器可访问的上层字母（例如 'A'）*/
    #define LV_FS_FATFS_CACHE_SIZE 0    /*>0 以在 lv_fs_read() 中缓存此数量的字节*/
#endif

/*API for LittleFS (库需要单独添加)。使用 lfs_file_open, lfs_file_read, etc*/
#define LV_USE_FS_LITTLEFS 0
#if LV_USE_FS_LITTLEFS
    #define LV_FS_LITTLEFS_LETTER '\0'     /*设置驱动器可访问的上层字母（例如 'A'）*/
    #define LV_FS_LITTLEFS_CACHE_SIZE 0    /*>0 以在 lv_fs_read() 中缓存此数量的字节*/
#endif

/*PNG 解码库*/
#define LV_USE_PNG 0

/*BMP 解码库*/
#define LV_USE_BMP 0

/*JPG + split JPG 解码库。
 *Split JPG 是为嵌入式系统优化的自定义格式。*/
#define LV_USE_SJPG 0

/*GIF 解码库*/
#define LV_USE_GIF 0

/*QR 码库*/
#define LV_USE_QRCODE 0

/*FreeType 库*/
#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
    /*FreeType 用于缓存字符的内存 [字节] (-1: 无缓存)*/
    #define LV_FREETYPE_CACHE_SIZE (16 * 1024)
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /*1: 位图缓存使用 sbit 缓存, 0: 位图缓存使用图像缓存。*/
        /*sbit 缓存：对于小位图（字体大小 < 256）非常节省内存 */
        /*如果字体大小 >= 256，必须配置为图像缓存 */
        #define LV_FREETYPE_SBIT_CACHE 0
        /*此缓存实例管理的已打开 FT_Face/FT_Size 对象的最大数量。*/
        /*(0: 使用系统默认值) */
        #define LV_FREETYPE_CACHE_FT_FACES 0
        #define LV_FREETYPE_CACHE_FT_SIZES 0
    #endif
#endif

/*Tiny TTF 库*/
#define LV_USE_TINY_TTF 0
#if LV_USE_TINY_TTF
    /*从文件加载 TTF 数据*/
    #define LV_TINY_TTF_FILE_SUPPORT 0
#endif

/*Rlottie 库*/
#define LV_USE_RLOTTIE 0

/*FFmpeg 库用于图像解码和播放视频
 *支持所有主要图像格式，因此不要与其他图像解码器一起启用*/
#define LV_USE_FFMPEG 0
#if LV_USE_FFMPEG
    /*将输入信息转储到 stderr*/
    #define LV_FFMPEG_DUMP_FORMAT 0
#endif

/*-----------
 * Others
 *----------*/

/*1: 启用 API 以对对象进行快照*/
#define LV_USE_SNAPSHOT 0

/*1: 启用 Monkey 测试*/
#define LV_USE_MONKEY 0

/*1: 启用网格导航*/
#define LV_USE_GRIDNAV 0

/*1: 启用 lv_obj 片段*/
#define LV_USE_FRAGMENT 0

/*1: 支持在标签或 span 小部件中使用图像作为字体 */
#define LV_USE_IMGFONT 0

/*1: 启用基于发布订阅的消息系统 */
#define LV_USE_MSG 0

/*1: 启用拼音输入法*/
/*需要：lv_keyboard*/
#define LV_USE_IME_PINYIN 0
#if LV_USE_IME_PINYIN
    /*1: 使用默认词典*/
    /*如果不使用默认词典，请确保在设置词典后使用 `lv_ime_pinyin`*/
    #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
    /*设置可以显示的最大候选面板数量*/
    /*这需要根据屏幕大小进行调整*/
    #define LV_IME_PINYIN_CAND_TEXT_NUM 6

    /*使用 9 键输入(k9)*/
    #define LV_IME_PINYIN_USE_K9_MODE      1
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif // LV_IME_PINYIN_USE_K9_MODE
#endif

/*==================
* EXAMPLES
*==================*/

/*启用示例以与库一起构建*/
#define LV_BUILD_EXAMPLES 1

/*===================
 * DEMO USAGE
 ====================*/

/*显示一些小部件。可能需要增加 `LV_MEM_SIZE` */
#define LV_USE_DEMO_WIDGETS 0
#if LV_USE_DEMO_WIDGETS
#define LV_DEMO_WIDGETS_SLIDESHOW 0
#endif

/*演示编码器和键盘的使用*/
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 1

/*基准测试您的系统*/
#define LV_USE_DEMO_BENCHMARK 0
#if LV_USE_DEMO_BENCHMARK
/*使用 RGB565A8 图像，颜色深度为 16 位，而不是 ARGB8565*/
#define LV_DEMO_BENCHMARK_RGB565A8 0
#endif

/*LVGL 的压力测试*/
#define LV_USE_DEMO_STRESS 0

/*音乐播放器演示*/
#define LV_USE_DEMO_MUSIC 0
#if LV_USE_DEMO_MUSIC
    #define LV_DEMO_MUSIC_SQUARE    1
    #define LV_DEMO_MUSIC_LANDSCAPE 1
    #define LV_DEMO_MUSIC_ROUND     1
    #define LV_DEMO_MUSIC_LARGE     0
    #define LV_DEMO_MUSIC_AUTO_PLAY 1
#endif

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*结束 "Content enable"*/
