/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "./../lvgl_bsp.h"
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES 320
#define MY_DISP_VER_RES 240

#ifndef MY_DISP_HOR_RES
#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
#define MY_DISP_HOR_RES 320
#endif

#ifndef MY_DISP_VER_RES
#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
#define MY_DISP_VER_RES 240
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
// static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//         const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#define LVGL_FLUSH_ROWS MY_DISP_VER_RES
static void lvgl_wait_cb(struct _lv_disp_drv_t *disp_drv)
{
    os_sleep(1);
}
void lv_port_disp_init(void)
{
    disp_init();

    static lv_disp_draw_buf_t draw_buf_dsc_3 __section_sdram;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * LVGL_FLUSH_ROWS] __section_sdram; /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * LVGL_FLUSH_ROWS] __section_sdram; /*Another screen sized buffer*/
    memset(&draw_buf_dsc_3, 0, sizeof(draw_buf_dsc_3));
    memset(buf_3_1, 0, sizeof(buf_3_1));
    memset(buf_3_2, 0, sizeof(buf_3_2));
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_HOR_RES * LVGL_FLUSH_ROWS); /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv __section_sdram; /*Descriptor of a display driver*/
    memset(&disp_drv, 0, sizeof(disp_drv));
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf_dsc_3;
    disp_drv.full_refresh = 0;
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET)
    // {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    // int32_t x;
    // int32_t y;
    // for (y = area->y1; y <= area->y2; y++)
    // {
    //     for (x = area->x1; x <= area->x2; x++)
    //     {
    //         /*Put a pixel to the display. For example:*/
    //         /*put_px(x, y, *color_p)*/
    //         color_p++;
    //     }
    // }

    gc9307c_write(&g_gc9307c_lcd, area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    // }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
// static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                     const lv_area_t * fill_area, lv_color_t color)
//{
//     /*It's an example code which should be done by your GPU*/
//     int32_t x, y;
//     dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//     for(y = fill_area->y1; y <= fill_area->y2; y++) {
//         for(x = fill_area->x1; x <= fill_area->x2; x++) {
//             dest_buf[x] = color;
//         }
//         dest_buf+=dest_width;    /*Go to the next line*/
//     }
// }

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
