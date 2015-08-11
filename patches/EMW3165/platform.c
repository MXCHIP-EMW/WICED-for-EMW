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
  /* Common GPIOs for internal use */
  [WICED_SYS_LED]                      = { GPIOB,  10 }, 
  [WICED_RF_LED]                       = { GPIOA,  4 }, 
  [BOOT_SEL]                          = { GPIOB,  1 }, 
  [MFG_SEL]                           = { GPIOB,  0 }, 
  [EasyLink_BUTTON]                   = { GPIOA,  1 }, 
  [STDIO_UART_RX]                     = { GPIOA,  3 },  
  [STDIO_UART_TX]                     = { GPIOA,  2 },  
  [FLASH_PIN_SPI_CS  ]                = { GPIOA, 15 },
  [FLASH_PIN_SPI_CLK ]                = { GPIOB,  3 },
  [FLASH_PIN_SPI_MOSI]                = { GPIOA,  7 },
  [FLASH_PIN_SPI_MISO]                = { GPIOB,  4 },

  /* GPIOs for external use */
  [WICED_GPIO_2]                       = { GPIOB,  2 },
  [WICED_GPIO_8]                       = { GPIOA , 2 },
  [WICED_GPIO_9]                       = { GPIOA,  1 },
  [WICED_GPIO_12]                      = { GPIOA,  3 },
  [WICED_GPIO_16]                      = { GPIOC, 13 },
  [WICED_GPIO_17]                      = { GPIOB, 10 },
  [WICED_GPIO_18]                      = { GPIOB,  9 },
  [WICED_GPIO_19]                      = { GPIOB, 12 },
  [WICED_GPIO_27]                      = { GPIOA, 12 },  
  [WICED_GPIO_29]                      = { GPIOA, 10 },
  [WICED_GPIO_30]                      = { GPIOB,  6 },
  [WICED_GPIO_31]                      = { GPIOB,  8 },
  [WICED_GPIO_33]                      = { GPIOB, 13 },
  [WICED_GPIO_34]                      = { GPIOA,  5 },
  [WICED_GPIO_35]                      = { GPIOA, 10 },
  [WICED_GPIO_36]                      = { GPIOB,  1 },
  [WICED_GPIO_37]                      = { GPIOB,  0 },
  [WICED_GPIO_38]                      = { GPIOA,  4 },
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
    .port                         = SPI1,
    .gpio_af                      = GPIO_AF_SPI1,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI1,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[FLASH_PIN_SPI_MOSI],
    .pin_miso                     = &platform_gpio_pins[FLASH_PIN_SPI_MISO],
    .pin_clock                    = &platform_gpio_pins[FLASH_PIN_SPI_CLK],
    .tx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream5,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 ),
    },
    .rx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream0,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream0_IRQn,
      .complete_flags             = DMA_LISR_TCIF0,
      .error_flags                = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
    },
  }
};


/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
  [WICED_UART_1] =
  {
    .port                         = USART2,
    .tx_pin                       = &platform_gpio_pins[STDIO_UART_TX],
    .rx_pin                       = &platform_gpio_pins[STDIO_UART_RX],
    .cts_pin                      = NULL,
    .rts_pin                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream6,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream5,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
  },
  [WICED_UART_2] =
  {
    .port                         = USART1,
    .tx_pin                       = &platform_gpio_pins[WICED_GPIO_30],
    .rx_pin                       = &platform_gpio_pins[WICED_GPIO_29],
    .cts_pin                      = NULL,
    .rts_pin                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream7,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream7_IRQn,
      .complete_flags             = DMA_HISR_TCIF7,
      .error_flags                = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream2,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream2_IRQn,
      .complete_flags             = DMA_LISR_TCIF2,
      .error_flags                = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
    },
  },
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* I2S peripherals. Used by WICED/platform/MCU/STM32F4xx/peripherals/wiced_platform_common.c*/
// Need to match this to the datasheet!
const platform_i2s_t i2s_interfaces[] =
{
    [WICED_I2S_1] =
    {
        .spi                          = SPI2,
#ifdef DAC_EXTERNAL_OSCILLATOR
        .is_master                    = 0,
        .enable_mclk                  = 0,
#else /* DAC_EXTERNAL_OSCILLATOR */
        .is_master                    = 1,
        .enable_mclk                  = 1,
#endif /* DAC_EXTERNAL_OSCILLATOR */
        .gpio_af                      = GPIO_AF_SPI2,
        .peripheral_clock             = RCC_APB1Periph_SPI2,
        .peripheral_clock_func        = RCC_APB1PeriphClockCmd,
        .pin_ck                       = &platform_gpio_pins[WICED_GPIO_24],
        .pin_sd                       = &platform_gpio_pins[WICED_GPIO_25],
        .pin_ws                       = &platform_gpio_pins[WICED_GPIO_23],
        .pin_mclk                     = &platform_gpio_pins[WICED_GPIO_18],
        .tx_dma.dma_register          = DMA1,
        .tx_dma.stream                = DMA1_Stream4,
        .tx_dma.channel               = DMA_Channel_0,
        .tx_dma.peripheral_clock      = RCC_AHB1Periph_DMA1,
        .tx_dma.peripheral_clock_func = RCC_AHB1PeriphClockCmd,
        .tx_dma.irq                   = DMA1_Stream4_IRQn,
        .rx_dma.dma_register          = DMA1,
        .rx_dma.stream                = DMA1_Stream3,
        .rx_dma.channel               = DMA_Channel_0,
        .rx_dma.peripheral_clock      = RCC_AHB1Periph_DMA1,
        .rx_dma.peripheral_clock_func = RCC_AHB1PeriphClockCmd,
        .rx_dma.irq                   = DMA1_Stream3_IRQn,
    }
};

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


/*
const wiced_spi_device_t wiced_nfc_device =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_GPIO_5,
    .speed       = 5000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
*/

/*
const platform_flash_t platform_flash_peripherals[] =
{
  [WICED_SPI_FLASH] =
  {
    .flash_type                   = FLASH_TYPE_SPI,
    .flash_start_addr             = 0x000000,
    .flash_length                 = 0x200000,
  },
  [WICED_INTERNAL_FLASH] =
  {
    .flash_type                   = FLASH_TYPE_INTERNAL,
    .flash_start_addr             = 0x08000000,
    .flash_length                 = 0x80000,
  },
};

platform_flash_driver_t platform_flash_drivers[WICED_FLASH_MAX];
*/

/* SPI flash. Exposed to the applications through include/wiced_platform.h */
#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
  .port        = WICED_SPI_1,
  .chip_select = WICED_SPI_FLASH_CS,
  .speed       = 40000000,
  .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_USE_DMA | SPI_MSB_FIRST ),
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

/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
* SDIO: EMW1062_PIN_BOOTSTRAP[1:0] = b'00
* gSPI: EMW1062_PIN_BOOTSTRAP[1:0] = b'01
*/
const platform_gpio_t wifi_control_pins[] =
{
  [WWD_PIN_RESET]           = { GPIOB, 14 },
};

/* Wi-Fi SDIO bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
  [WWD_PIN_SDIO_OOB_IRQ] = { GPIOA,  0 },
  [WWD_PIN_SDIO_CLK    ] = { GPIOB, 15 },
  [WWD_PIN_SDIO_CMD    ] = { GPIOA,  6 },
  [WWD_PIN_SDIO_D0     ] = { GPIOB,  7 },
  [WWD_PIN_SDIO_D1     ] = { GPIOA,  8 },
  [WWD_PIN_SDIO_D2     ] = { GPIOA,  9 },
  [WWD_PIN_SDIO_D3     ] = { GPIOB,  5 },
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
        Also, if WICED_BUTTON1 and WICED_BUTTON2 are not available, you are
        unable to use the bootloader patch to halt execution in the bootloader,
        a meathod to allow you to use JTAG for flashing even if you use those
        pins for other things.
    */

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
wiced_bool_t platform_check_factory_reset( void )
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
        if ( factory_reset_counter == 5000 )
        {
            return WICED_TRUE;
        }
    }
    return WICED_FALSE;
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
