/**
******************************************************************************
* @file    platform_config.h
* @author  William Xu, edited by Emil Fridriksson
* @version V1.0.1
* @date    22-08-2015
* @brief   Clock and peripheral configuration for MXCHIPs EMW3165
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_CLOCK_HZ            (100000000 )

#define HSE_SOURCE              ( RCC_HSE_ON )
#define AHB_CLOCK_DIVIDER       ( RCC_SYSCLK_Div1 )
#define APB1_CLOCK_DIVIDER      ( RCC_HCLK_Div2 )
#define APB2_CLOCK_DIVIDER      ( RCC_HCLK_Div1 )
#define PLL_SOURCE              ( RCC_PLLSource_HSE )
#define PLL_M_CONSTANT          ( 26 )
#define PLL_N_CONSTANT          ( 200 )
#define PLL_P_CONSTANT          ( 2 )
#define PPL_Q_CONSTANT          ( 4 )
#define SYSTEM_CLOCK_SOURCE     ( RCC_SYSCLKSource_PLLCLK )
#define SYSTICK_CLOCK_SOURCE    ( SysTick_CLKSource_HCLK )
#define INT_FLASH_WAIT_STATE    ( FLASH_Latency_3 )
#define PLATFORM_STM32_VOLTAGE_2V7_TO_3V6


/******************************************************
 *  Wi-Fi Options
 ******************************************************/

/*  GPIO pins are used to bootstrap Wi-Fi to SDIO or gSPI mode */
#define WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
#define WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_1
 
/*  Wi-Fi GPIO0 pin is used for out-of-band interrupt */
#define WICED_WIFI_OOB_IRQ_GPIO_PIN  ( 0 )

/*  Wi-Fi power pin is present */
#define WICED_USE_WIFI_POWER_PIN
 
/*  Wi-Fi reset pin is present */
#define WICED_USE_WIFI_RESET_PIN

/* */
#define WICED_USE_WIFI_32K_CLOCK_MCO

/*  OTA */
#define PLATFORM_HAS_OTA

#ifdef __cplusplus
} /* extern "C" */
#endif
