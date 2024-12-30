/**
 * @file msp_hal.h
 * @author WittXie
 * @brief 芯片支持包环境
 * @version 0.1
 * @date 2023-03-30
 * @details 填写所有需要的底层环境
 * @copyright Copyright (c) 2023
 */
#pragma once

// C语言标准库
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// stm32h750
#include "./../Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h"
#include "FreeRTOSConfig.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "fmc.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "main.h"
#include "quadspi.h"
#include "rtc.h"
#include "sdmmc.h"
#include "spi.h"
#include "stm32h7xx_hal_conf.h"
#include "stm32h7xx_it.h"
#include "tim.h"
#include "usart.h"

// freertos
#include "cmsis_os.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"
#include "timers.h"

// storage
#include "fatfs.h"
#include "ff_gen_drv.h"

// USB
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"
