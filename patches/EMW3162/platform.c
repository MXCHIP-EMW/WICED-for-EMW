#include "platform.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "wwd_platform_common.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wiced_platform.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define PLATFORM_FACTORY_RESET_CHECK_PERIOD     ( 100 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 5000 )

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* GPIO pin table. Used by WICED/platform/MCU/wiced_platform_common.c */

const platform_gpio_t platform_gpio_pins[] =
{
  /* GPIOs numbered like the pins on EMW3162 */
  [WICED_GPIO_0]  = { GPIOB,  0 }, // Green LED
  [WICED_GPIO_1]  = { GPIOB,  6 }, // I2C1_SCL
  [WICED_GPIO_2]  = { GPIOB,  7 }, // I2C1_SDA
  [WICED_GPIO_3]  = { GPIOA, 13 }, // SWDIO
  [WICED_GPIO_4]  = { GPIOC,  7 }, 
  [WICED_GPIO_5]  = { GPIOA,  3 }, // WPS BTN
  [WICED_GPIO_6]  = { GPIOA,  4 }, // SPI1_CS
  [WICED_GPIO_7]  = { GPIOB,  3 }, // SPI1_SCK -- JTDO/TRACESWO
  [WICED_GPIO_8]  = { GPIOB,  4 }, // SPI MISO -- NJTRST
  [WICED_GPIO_9]  = { GPIOB,  5 }, // SPI1_MOSI
  [WICED_GPIO_10] = { GPIOB,  8 },
  [WICED_GPIO_11] = { GPIOA,  1 }, // Easylink BTN
  [WICED_GPIO_12] = { GPIOC,  2 },
  [WICED_GPIO_13] = { GPIOB, 14 },
  [WICED_GPIO_14] = { GPIOC,  6 }, // Sleep SW
  // [WICED_GPIO_15] is GND
  [WICED_GPIO_16] = { GPIOB,  1 }, // Red LED and Boot SW
  // [WICED_GPIO_17] is nRESET     // Reset BTN
  [WICED_GPIO_18] = { GPIOA, 15 }, // JTDI
  [WICED_GPIO_19] = { GPIOB, 11 },
  [WICED_GPIO_20] = { GPIOA, 12 },
  [WICED_GPIO_21] = { GPIOA, 11 },
  [WICED_GPIO_22] = { GPIOA,  9 },
  [WICED_GPIO_23] = { GPIOA, 10 },
  // [WICED_GPIO_24] is VCC
  // [WICED_GPIO_25] is GND
  // [WICED_GPIO_26] is NC
  // [WICED_GPIO_27] is BOOT0
  [WICED_GPIO_28] = { GPIOA, 14 }, // JTCK- SWCLK
  [WICED_GPIO_29] = { GPIOA,  0 }, // Standby SW -- PA0-WKUP
  [WICED_GPIO_30] = { GPIOB,  9 }, // Status SW
  [WICED_GPIO_31] = { GPIOA,  5 },
  [WICED_GPIO_32] = { GPIOA,  6 },
  [WICED_GPIO_33] = { GPIOA,  7 }, // NOTE: Some schematics and docs incorrectly refer to this pin as
                                   // PA8. Do not attempt to use PA8, as it's used for the SDIO bus to 
                                   // the Wifi module.
  [WICED_GPIO_34] = { GPIOB, 15 },
  [WICED_GPIO_35] = { GPIOC,  3 },
  [WICED_GPIO_36] = { GPIOC,  4 },
  // 37 - 44 are GND or NC
};

/* ADC peripherals. Used WICED/platform/MCU/wiced_platform_common.c */
const platform_adc_t platform_adc_peripherals[] =
{
    [WICED_ADC_1] = {ADC1, ADC_Channel_3, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_5]},
    [WICED_ADC_2] = {ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_6]},
    [WICED_ADC_3] = {ADC1, ADC_Channel_1, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_11]},
    [WICED_ADC_4] = {ADC1, ADC_Channel_12, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_12]},
    [WICED_ADC_5] = {ADC1, ADC_Channel_9, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_16]},
    [WICED_ADC_6] = {ADC1, ADC_Channel_0, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_29]},
    [WICED_ADC_7] = {ADC1, ADC_Channel_5, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_31]},
    [WICED_ADC_8] = {ADC1, ADC_Channel_6, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_32]},
    [WICED_ADC_9] = {ADC1, ADC_Channel_13, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_35]},
    [WICED_ADC_10] = {ADC1, ADC_Channel_14, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_36]},
};

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {TIM4, 1, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, &platform_gpio_pins[WICED_GPIO_0]},
    [WICED_PWM_2]  = {TIM4, 2, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, &platform_gpio_pins[WICED_GPIO_16]},
//     [WICED_PWM_3]  = {TIM2, 2, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_2] }, /* or TIM5/Ch2                       */
//     [WICED_PWM_4]  = {TIM2, 3, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_3] }, /* or TIM5/Ch3, TIM9/Ch1             */
//     [WICED_PWM_5]  = {TIM2, 4, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_4] }, /* or TIM5/Ch4, TIM9/Ch2             */
//     [WICED_PWM_6]  = {TIM2, 1, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_6] }, /* or TIM2_CH1_ETR, TIM8/Ch1N        */
//     [WICED_PWM_7]  = {TIM3, 1, RCC_APB1Periph_TIM3, GPIO_AF_TIM3, &platform_gpio_pins[WICED_GPIO_7] }, /* or TIM1_BKIN, TIM8_BKIN, TIM13/Ch1*/
//     [WICED_PWM_8]  = {TIM3, 2, RCC_APB1Periph_TIM3, GPIO_AF_TIM3, &platform_gpio_pins[WICED_GPIO_8] }, /* or TIM8/Ch1N, TIM14/Ch1           */
//     [WICED_PWM_9]  = {TIM5, 2, RCC_APB1Periph_TIM5, GPIO_AF_TIM5, &platform_gpio_pins[WICED_GPIO_2] }, /* or TIM2/Ch2                       */
};

/* SPI peripherals */
const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                  = SPI1,
        .gpio_af               = GPIO_AF_SPI1,
        .peripheral_clock_reg  = RCC_APB2Periph_SPI1,
        .peripheral_clock_func = RCC_APB2PeriphClockCmd,
        .pin_mosi              = &platform_gpio_pins[WICED_GPIO_9],
        .pin_miso              = &platform_gpio_pins[WICED_GPIO_8],
        .pin_clock             = &platform_gpio_pins[WICED_GPIO_7],
        .tx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream5,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream5_IRQn,
            .complete_flags    = DMA_HISR_TCIF5,
            .error_flags       = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
        },
        .rx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream0,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream0_IRQn,
            .complete_flags    = DMA_LISR_TCIF0,
            .error_flags       = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
        },
    }
};

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_1] =
    {
        .port               = USART1,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_22 ],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_23 ],
        .cts_pin            = NULL,
        .rts_pin            = NULL,
        .tx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream7,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream7_IRQn,
            .complete_flags = DMA_HISR_TCIF7,
            .error_flags    = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream2,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream2_IRQn,
            .complete_flags = DMA_LISR_TCIF2,
            .error_flags    = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
        },
    },
    [WICED_UART_2] =
    {
        .port               = USART6,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_14],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_4],
        .cts_pin            = NULL,
        .rts_pin            = NULL,
        .tx_dma_config =
        {
            .controller     = DMA1,
            .stream         = DMA1_Stream6,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA1_Stream6_IRQn,
            .complete_flags = DMA_HISR_TCIF6,
            .error_flags    = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA1,
            .stream         = DMA1_Stream5,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA1_Stream5_IRQn,
            .complete_flags = DMA_HISR_TCIF5,
            .error_flags    = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
        },
    },
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];



/* I2C peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = I2C1,
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_1],
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_2],
        .peripheral_clock_reg    = RCC_APB1Periph_I2C1,
        .tx_dma                  = DMA1,
        .tx_dma_peripheral_clock = RCC_AHB1Periph_DMA1,
        .tx_dma_stream           = DMA1_Stream7,
        .rx_dma_stream           = DMA1_Stream5,
        .tx_dma_stream_id        = 7,
        .rx_dma_stream_id        = 5,
        .tx_dma_channel          = DMA_Channel_1,
        .rx_dma_channel          = DMA_Channel_1,
        .gpio_af                 = GPIO_AF_I2C1
    },
};


const wiced_spi_device_t wiced_nfc_device =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_GPIO_5,
    .speed       = 5000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};


/* SPI flash. Exposed to the applications through include/wiced_platform.h */
// Not on the EMW3162. Needed for WICED OTA updates.
#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_SPI_FLASH_CS,
    .speed       = 5000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
#endif

/* UART standard I/O configuration */
#ifndef WICED_DISABLE_STDIO
static platform_uart_config_t stdio_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};
#endif

/* Wi-Fi control pins. Used by WICED/platform/MCU/wwd_platform_common.c
 * SDIO: WWD_PIN_BOOTSTRAP[1:0] = b'00
 * gSPI: WWD_PIN_BOOTSTRAP[1:0] = b'01
 */
const platform_gpio_t wifi_control_pins[] =
{
    [WWD_PIN_POWER      ] = { GPIOC,  1 },
    [WWD_PIN_RESET      ] = { GPIOC,  5 },
#if defined ( WICED_USE_WIFI_32K_CLOCK_MCO )
    [WWD_PIN_32K_CLK    ] = { GPIOA,  8 },
#else
    [WWD_PIN_32K_CLK    ] = { GPIOA, 11 },
#endif
    [WWD_PIN_BOOTSTRAP_0] = { GPIOB,  12 },
    [WWD_PIN_BOOTSTRAP_1] = { GPIOB,  13 },
};

/* Wi-Fi SDIO bus pins. Used by WICED/platform/STM32F2xx/WWD/wwd_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
    [WWD_PIN_SDIO_OOB_IRQ] = { GPIOB, 13 },
    [WWD_PIN_SDIO_CLK    ] = { GPIOC, 12 },
    [WWD_PIN_SDIO_CMD    ] = { GPIOD,  2 },
    [WWD_PIN_SDIO_D0     ] = { GPIOC,  8 },
    [WWD_PIN_SDIO_D1     ] = { GPIOC,  9 },
    [WWD_PIN_SDIO_D2     ] = { GPIOC, 10 },
    [WWD_PIN_SDIO_D3     ] = { GPIOC, 11 },
};

/* Wi-Fi gSPI bus pins. Used by WICED/platform/STM32F2xx/WWD/wwd_SPI.c */
const platform_gpio_t wifi_spi_pins[] =
{
    [WWD_PIN_SPI_IRQ ] = { GPIOC,  9 },
    [WWD_PIN_SPI_CS  ] = { GPIOC, 11 },
    [WWD_PIN_SPI_CLK ] = { GPIOB, 13 },
    [WWD_PIN_SPI_MOSI] = { GPIOB, 15 },
    [WWD_PIN_SPI_MISO] = { GPIOB, 14 },
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
    /* Interrupt priority setup. Called by WICED/platform/MCU/STM32F2xx/platform_init.c */
    NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
    NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
    NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
    NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN SPI DMA        */
    NVIC_SetPriority( USART1_IRQn      ,  6 ); /* WICED_UART_1        */
    NVIC_SetPriority( USART2_IRQn      ,  6 ); /* WICED_UART_2        */
    NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* WICED_UART_1 TX DMA */
    NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* WICED_UART_1 RX DMA */
    NVIC_SetPriority( DMA1_Stream6_IRQn,  7 ); /* WICED_UART_2 TX DMA */
    NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* WICED_UART_2 RX DMA */
    NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
}

void platform_init_external_devices( void )
{
    /*  This function initializes the external interfaces for the platform.
        You could use this to initialize the peripherals you use.
        Note however that if you initialize alternative functionality of 
        JTAG and UART pins, you could lock yourself out of being able to flash
        new firmware.
    */
    
    //patch added to resolve the microseconds delay hang issue.
    do
    {
        // enable DWT hardware and cycle counting
        CoreDebug->DEMCR = CoreDebug->DEMCR | CoreDebug_DEMCR_TRCENA_Msk;
        // reset a counter
        DWT->CYCCNT = 0;
        // enable the counter
        DWT->CTRL = (DWT->CTRL | DWT_CTRL_CYCCNTENA_Msk) ;
    }
    while(0);

    /* Initialise LEDs and turn off by default */
    platform_gpio_init( &platform_gpio_pins[WICED_LED1], OUTPUT_PUSH_PULL );
    platform_gpio_init( &platform_gpio_pins[WICED_LED2], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED1] );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED2] );

    /* Initialise buttons to input by default */
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON1], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON2], INPUT_PULL_UP );
    //platform_gpio_init( &platform_gpio_pins[WICED_SWITCH1], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_SWITCH2], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_SWITCH3], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_SWITCH4], INPUT_PULL_UP );


#ifndef WICED_DISABLE_STDIO
    /* Initialise UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}

/* Checks if a factory reset is requested */
uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time )
{
    uint32_t factory_reset_counter = 0;
    int led_state = 0;
    while (  ( 0 == platform_gpio_input_get( &platform_gpio_pins[ WICED_BUTTON1 ] ) )
           &&( ( factory_reset_counter += PLATFORM_FACTORY_RESET_CHECK_PERIOD ) <= PLATFORM_FACTORY_RESET_TIMEOUT )
           &&( WICED_SUCCESS == (wiced_result_t)host_rtos_delay_milliseconds( PLATFORM_FACTORY_RESET_CHECK_PERIOD ) )
          )
    {
        /* Factory reset button is being pressed. */
        /* User Must press it for 5 seconds to ensure it was not accidental */
        /* Toggle LED every 100ms */

        if ( led_state == 0 )
        {
            platform_gpio_output_high( &platform_gpio_pins[ WICED_LED1 ] );
            led_state = 1;
        }
        else
        {
            platform_gpio_output_low( &platform_gpio_pins[ WICED_LED1 ] );
            led_state = 0;
        }
        if ( factory_reset_counter >= max_time )
        {
#ifdef emw3162_bootloader_patch
            if (!platform_gpio_input_get( &platform_gpio_pins[ WICED_BUTTON2 ]) while(1);
#endif
            break;
        }
    }

    return factory_reset_counter;
}

/******************************************************
 *           Interrupt Handler Definitions
 ******************************************************/

WWD_RTOS_DEFINE_ISR( usart1_irq )
{
    platform_uart_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_irq )
{
    platform_uart_irq( &platform_uart_drivers[WICED_UART_2] );
}

WWD_RTOS_DEFINE_ISR( usart1_tx_dma_irq )
{
    platform_uart_tx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_tx_dma_irq )
{
    platform_uart_tx_dma_irq( &platform_uart_drivers[WICED_UART_2] );
}

WWD_RTOS_DEFINE_ISR( usart1_rx_dma_irq )
{
    platform_uart_rx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_rx_dma_irq )
{
    platform_uart_rx_dma_irq( &platform_uart_drivers[WICED_UART_2] );
}

/******************************************************
 *            Interrupt Handlers Mapping
 ******************************************************/

/* These DMA assignments can be found STM32F2xx datasheet DMA section */
WWD_RTOS_MAP_ISR( usart1_irq       , USART1_irq       )
WWD_RTOS_MAP_ISR( usart1_tx_dma_irq, DMA2_Stream7_irq )
WWD_RTOS_MAP_ISR( usart1_rx_dma_irq, DMA2_Stream2_irq )
WWD_RTOS_MAP_ISR( usart2_irq       , USART2_irq       )
WWD_RTOS_MAP_ISR( usart2_tx_dma_irq, DMA1_Stream6_irq )
WWD_RTOS_MAP_ISR( usart2_rx_dma_irq, DMA1_Stream5_irq )
