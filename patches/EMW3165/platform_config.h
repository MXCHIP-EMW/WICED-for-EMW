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
//#define WICED_USE_WIFI_POWER_PIN
 
/*  Wi-Fi reset pin is present */
#define WICED_USE_WIFI_RESET_PIN

/* */
#define WICED_USE_WIFI_32K_CLOCK_MCO

/*  OTA */
#define PLATFORM_HAS_OTA

#ifdef __cplusplus
} /* extern "C" */
#endif
