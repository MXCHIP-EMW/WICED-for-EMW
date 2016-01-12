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
	//[WICED_GPIO_1]										// Not connected
	[WICED_GPIO_2]                       = { GPIOB,   2 },	// BOOT1
	//[WICED_GPIO_3]										// Not connected

	// SPI1 - FLASH dedicated SPI bus. Do not use!
	[WICED_GPIO_4]                       = { GPIOA,   7 },	// Flash_SPI1_MOSI
	[WICED_GPIO_5]                       = { GPIOA,  15 },	// Flash_SPI1_NSS
	[WICED_GPIO_6]                       = { GPIOB,   3 },	// Flash_SPI1_SCK
	[WICED_GPIO_7]                       = { GPIOB,   4 },	// Flash_SPI1_MISO
	// End of SPI1 bus

	[WICED_GPIO_8]                       = { GPIOA,   2 },	// TIM2_CH3,TIM5_CH3,TIM9_CH1,I2S2_CKIN,USART2_TX,ADC1_2
	[WICED_GPIO_9]                       = { GPIOA,   1 },	// TIM2_CH2,TIM5_CH2,SPI4_MOSI/I2S4_SD,USART2_RTS,ADC1_1
	//[WICED_GPIO_10]										// VBAT - Connect to VDD if not in use.
	//[WICED_GPIO_11]										// Not connected
	[WICED_GPIO_12]                      = { GPIOA,   3 },	// TIM2_CH4,TIM5_CH4,TIM9_CH2,I2S2_MCK,USART2_RX,ADC1_3
	//[WICED_GPIO_13]										// NRST - RESET
	[WICED_GPIO_14]                      = { GPIOA,   0 },	// Wi-Fi wake up MCU
	//[WICED_GPIO_15]										// Not connected
	[WICED_GPIO_16]                      = { GPIOC,  13 },	// RTC_AMP1, RTC_OUT, RTC_TS
	[WICED_GPIO_17]                      = { GPIOB,  10 },	// TIM2_CH3,I2C2_SCL,SPI2_SCK/I2S2_CK,I2S3_MCK
	[WICED_GPIO_18]                      = { GPIOB,   9 },	// TIM4_CH4,TIM11_CH1,I2C1_SDA,SPI2_NSS/I2S2_WS,I2C2_SDA
	[WICED_GPIO_19]                      = { GPIOB,  12 },	// TIM1_BKIN,I2C2_SMBA,SPI2_NSS/I2S2_WS,SPI4_NSS/I2S4_WS,SPI3_SCK/I2S3_CK
	//[WICED_GPIO_2O]										// GND
	//[WICED_GPIO_21]										// GND
	//[WICED_GPIO_22]										// Not connected
	//[WICED_GPIO_23]										// Not connected
	//[WICED_GPIO_24]										// Not connected

	// NOTE: Using those pins for anything other than debugging could make your life difficult
	[WICED_GPIO_25]                      = { GPIOA,  14 },	// SWD_SWCLK
	[WICED_GPIO_26]                      = { GPIOA,  13 },	// SWD_SWDIO
	// END NOTE

	[WICED_GPIO_27]                      = { GPIOA,  12 },	// TIM1_ETR,SPI5_MISO,USART1_RTS,USART6_RX,USB_FS_DP
	//[WICED_GPIO_28]										// Not connected
	[WICED_GPIO_29]                      = { GPIOA,  10 },	// TIM1_CH3,SPI5_MOSI/I2S5_SD,USART1_RX,USB_FS_ID
	[WICED_GPIO_30]                      = { GPIOB,   6 },	// TIM4_CH1,I2C1_SCL,USART1_TX
	[WICED_GPIO_31]                      = { GPIOB,   8 },	// TIM4_CH3,TIM10_CH1,I2C1_SCL,SPI5_MOSI/I2S5_SD,I2C3_SDA
	//[WICED_GPIO_32]										// Not connected
	[WICED_GPIO_33]                      = { GPIOB,  13 },	// TIM1_CH1N,SPI2_SCK/I2S2_CK,SPI4_SCK/I2S4_CK
	[WICED_GPIO_34]                      = { GPIOA,   5 },	// TIM2_CH1/TIM2_ET,SPI1_SCK/I2S1_CK,ADC1_5
	[WICED_GPIO_35]                      = { GPIOA,  11 },	// TIM1_CH4,SPI4_MISO,USART1_CTS,USART6_TX,USB_FS_DM
	[WICED_GPIO_36]                      = { GPIOB,   1 },	// TIM1_CH3N,TIM3_CH4,SPI5_NSS/I2S5_WS,ADC1_9
	[WICED_GPIO_37]                      = { GPIOB,   0 },	// TIM1_CH2N,TIM3_CH3,SPI5_SCK/I2S5_CK,ADC1_8
	[WICED_GPIO_38]                      = { GPIOA,   4 },	// SPI1_NSS/I2S1_WS,SPI3_NSS/I2S3_WS,USART2_CK,ADC1_4
	//[WICED_GPIO_39]										// VDD
	//[WICED_GPIO_40]										// VDD
	//[WICED_GPIO_41]										// ANT - External antenna pad

};

/* ADC peripherals. Used WICED/platform/MCU/wiced_platform_common.c */
const platform_adc_t platform_adc_peripherals[] =
{
    [WICED_ADC_1] = {ADC1, ADC_Channel_1, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_9]},
    [WICED_ADC_2] = {ADC1, ADC_Channel_2, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_8]},
    [WICED_ADC_3] = {ADC1, ADC_Channel_3, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_12]},
    [WICED_ADC_4] = {ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_38]},
    [WICED_ADC_5] = {ADC1, ADC_Channel_5, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_34]},
    [WICED_ADC_8] = {ADC1, ADC_Channel_8, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_37]},
    [WICED_ADC_9] = {ADC1, ADC_Channel_9, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_36]},
};

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
/* This is a sample config, refer to the datasheet to alternative configurations of timers and channels*/
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {TIM4, 1, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, &platform_gpio_pins[WICED_GPIO_30]},
    [WICED_PWM_2]  = {TIM4, 3, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, &platform_gpio_pins[WICED_GPIO_31]},
    [WICED_PWM_3]  = {TIM4, 4, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, &platform_gpio_pins[WICED_GPIO_18]},
    [WICED_PWM_4]  = {TIM2, 1, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_34]},
    [WICED_PWM_5]  = {TIM2, 2, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_9] },
    [WICED_PWM_6]  = {TIM2, 3, RCC_APB1Periph_TIM2, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_17]},
    [WICED_PWM_7]  = {TIM2, 4, RCC_APB1Periph_TIM4, GPIO_AF_TIM2, &platform_gpio_pins[WICED_GPIO_12]},
    [WICED_PWM_8]  = {TIM3, 3, RCC_APB1Periph_TIM3, GPIO_AF_TIM3, &platform_gpio_pins[WICED_GPIO_37]},
    [WICED_PWM_9]  = {TIM3, 4, RCC_APB1Periph_TIM4, GPIO_AF_TIM3, &platform_gpio_pins[WICED_GPIO_36]},
};

/* SPI peripherals */
/* NOTE: SPI peripherals SPI4, SPI5 and SPI6 might have issues with RX DMA
 *       See https://github.com/MXCHIP-EMW/WICED-for-EMW/pull/13
 *       and https://github.com/MXCHIP-EMW/WICED-for-EMW/issues/9
 */
const platform_spi_t platform_spi_peripherals[] =
{
  [WICED_SPI_1]  =
  {
    .port                         = SPI1,
    .gpio_af                      = GPIO_AF_SPI1,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI1,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[WICED_GPIO_4],
    .pin_miso                     = &platform_gpio_pins[WICED_GPIO_7],
    .pin_clock                    = &platform_gpio_pins[WICED_GPIO_6],
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
  },
  [WICED_SPI_2]  =
  {
    .port                         = SPI2,
    .gpio_af                      = GPIO_AF_SPI2,
    .peripheral_clock_reg         = RCC_APB1Periph_SPI2,
    .peripheral_clock_func        = RCC_APB1PeriphClockCmd,
//    .pin_mosi                     = &platform_gpio_pins[WICED_GPIO_X],
//    .pin_miso                     = &platform_gpio_pins[WICED_GPIO_X],
//    .pin_clock                    = &platform_gpio_pins[WICED_GPIO_X],
    .tx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream4,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream4_IRQn,
      .complete_flags             = DMA_HISR_TCIF4,
      .error_flags                = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 ),
    },
    .rx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream3,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream3_IRQn,
      .complete_flags             = DMA_LISR_TCIF3,
      .error_flags                = ( DMA_LISR_TEIF3 | DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 ),
    },
  },
  [WICED_SPI_3]  =
  {
    .port                         = SPI3,
    .gpio_af                      = GPIO_AF_SPI3,
    .peripheral_clock_reg         = RCC_APB1Periph_SPI3,
    .peripheral_clock_func        = RCC_APB1PeriphClockCmd,
//    .pin_mosi                     = &platform_gpio_pins[WICED_GPIO_X],
//    .pin_miso                     = &platform_gpio_pins[WICED_GPIO_X],
    .pin_clock                    = &platform_gpio_pins[WICED_GPIO_12],
    .tx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream5,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 ),
    },
    .rx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream0,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream0_IRQn,
      .complete_flags             = DMA_LISR_TCIF0,
      .error_flags                = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
    },
  },
  [WICED_SPI_4]  =
  {
    .port                         = SPI4,
    .gpio_af                      = GPIO_AF_SPI4,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI4,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[WICED_GPIO_9],
    .pin_miso                     = &platform_gpio_pins[WICED_GPIO_35],
    .pin_clock                    = &platform_gpio_pins[WICED_GPIO_33],
    .tx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream1,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream1_IRQn,
      .complete_flags             = DMA_LISR_TCIF1,
      .error_flags                = ( DMA_LISR_TEIF1 | DMA_LISR_FEIF1 ),
    },
    .rx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream4,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream4_IRQn,
      .complete_flags             = DMA_HISR_TCIF4,
      .error_flags                = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 | DMA_HISR_DMEIF4 ),
    },
  },
  [WICED_SPI_5]  =
  {
    .port                         = SPI5,
    .gpio_af                      = GPIO_AF_SPI5,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI5,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[WICED_GPIO_31],
    .pin_miso                     = &platform_gpio_pins[WICED_GPIO_27],
    .pin_clock                    = &platform_gpio_pins[WICED_GPIO_37],
    .tx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream6,
      .channel                    = DMA_Channel_7,
      .irq_vector                 = DMA2_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream5,
      .channel                    = DMA_Channel_7,
      .irq_vector                 = DMA2_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
  }
};


/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
  [WICED_UART_1] =
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
  [WICED_UART_2] =
  {
    .port                         = USART2,
    .tx_pin                       = &platform_gpio_pins[WICED_GPIO_8],
    .rx_pin                       = &platform_gpio_pins[WICED_GPIO_12],
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
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* I2S peripherals. Used by WICED/platform/MCU/STM32F4xx/peripherals/wiced_platform_common.c*/
// NOTE: I don't have a clue if this is configured correctly. Use at own risk.
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
        .pin_ck                       = &platform_gpio_pins[WICED_GPIO_17],
        .pin_sd                       = &platform_gpio_pins[WICED_GPIO_8],
        .pin_ws                       = &platform_gpio_pins[WICED_GPIO_18],
        .pin_mclk                     = &platform_gpio_pins[WICED_GPIO_12],
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
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_31],	// WICED_GPIO_30 works too, but
																		// this one is connected to the
																		// I2C bus on the dev board
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_18],
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

#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_SPI_FLASH_CS,
    .speed       = 20000000, // 50 MHz
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
#endif

/* UART standard I/O configuration */
#ifndef WICED_DISABLE_STDIO
static platform_uart_config_t stdio_config =
{
    .baud_rate    = 115200,				// TODO: Check if this is valid for USART2
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

    /* Initialise button to input by default */
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON1], INPUT_PULL_UP );

#ifndef GPIO_LED_NOT_SUPPORTED
    /* Initialise LEDs and turn off by default */
    platform_gpio_init( &platform_gpio_pins[WICED_LED1], OUTPUT_PUSH_PULL );
    platform_gpio_init( &platform_gpio_pins[WICED_LED2], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED1] );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED2] );
#endif

#ifndef WICED_DISABLE_STDIO
    /* Initialise UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}

/* Checks if a factory reset is requested */
wiced_bool_t platform_check_factory_reset( void )
{
    uint32_t factory_reset_counter = 0;
#ifndef GPIO_LED_NOT_SUPPORTED
    int led_state = 0;
#endif
    
    while (  ( 0 == platform_gpio_input_get( &platform_gpio_pins[ WICED_BUTTON1 ] ) )
           &&( ( factory_reset_counter += PLATFORM_FACTORY_RESET_CHECK_PERIOD ) <= PLATFORM_FACTORY_RESET_TIMEOUT )
           &&( WICED_SUCCESS == (wiced_result_t)host_rtos_delay_milliseconds( PLATFORM_FACTORY_RESET_CHECK_PERIOD ) )
          )
    {
        /* Factory reset button is being pressed. */
        /* User Must press it for 5 seconds to ensure it was not accidental */
#ifndef GPIO_LED_NOT_SUPPORTED
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
#endif
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
