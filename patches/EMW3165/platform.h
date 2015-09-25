/** @file
 * Defines peripherals available for use on EMW3165 board
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

// The MCU actually only supports 36 GPIOs, but this is handy.
typedef enum
{
    WICED_GPIO_1,
    WICED_GPIO_2,
    WICED_GPIO_3,
    WICED_GPIO_4,
    WICED_GPIO_5,
    WICED_GPIO_6,
    WICED_GPIO_7,
    WICED_GPIO_8,
    WICED_GPIO_9,
    WICED_GPIO_10,
    WICED_GPIO_11,
    WICED_GPIO_12,
    WICED_GPIO_13,
    WICED_GPIO_14,
    WICED_GPIO_15,
    WICED_GPIO_16,
    WICED_GPIO_17,
    WICED_GPIO_18,
    WICED_GPIO_19,
    WICED_GPIO_20,
    WICED_GPIO_21,
    WICED_GPIO_22,
    WICED_GPIO_23,
    WICED_GPIO_24,
    WICED_GPIO_25,
    WICED_GPIO_26,
    WICED_GPIO_27,
    WICED_GPIO_28,
    WICED_GPIO_29,
    WICED_GPIO_30,
    WICED_GPIO_31,
    WICED_GPIO_32,
    WICED_GPIO_33,
    WICED_GPIO_34,
    WICED_GPIO_35,
    WICED_GPIO_36,
    WICED_GPIO_37,
    WICED_GPIO_38,
    WICED_GPIO_39,
    WICED_GPIO_40,
    WICED_GPIO_41,
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    WICED_GPIO_32BIT = 0x7FFFFFFF,
} wiced_gpio_t;

// The datasheet says up to 5 SPIs
typedef enum
{
    WICED_SPI_1,
	WICED_SPI_2,
	WICED_SPI_3,
	WICED_SPI_4,
	WICED_SPI_5,
    WICED_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    WICED_SPI_32BIT = 0x7FFFFFFF,
} wiced_spi_t;

// The datasheet says up to 5 I2S
typedef enum 
{
    WICED_I2S_1,
	WICED_I2S_2,
	WICED_I2S_3,
	WICED_I2S_4,
	WICED_I2S_5,
    WICED_I2S_MAX,
    WICED_I2S_32BIT = 0x7FFFFFFF,
} wiced_i2s_t;

// The datasheet says up to 3 I2C
typedef enum
{
    WICED_I2C_1,
	WICED_I2C_2,
	WICED_I2C_3,
    WICED_I2C_MAX,
    WICED_I2C_32BIT = 0x7FFFFFFF,
} wiced_i2c_t;

// The datasheet says all 16 timers support PWM
typedef enum
{
    WICED_PWM_1,
    WICED_PWM_2,
    WICED_PWM_3,
    WICED_PWM_4,
    WICED_PWM_5,
    WICED_PWM_6,
    WICED_PWM_7,
    WICED_PWM_8,
    WICED_PWM_9,
	WICED_PWM_10,
	WICED_PWM_11,
	WICED_PWM_12,
	WICED_PWM_13,
	WICED_PWM_14,
	WICED_PWM_15,
	WICED_PWM_16,
    WICED_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    WICED_PWM_32BIT = 0x7FFFFFFF,
} wiced_pwm_t;

// The datasheet says up to 10 ADCs with the WLCSP49 package
typedef enum
{
    WICED_ADC_1,
    WICED_ADC_2,
    WICED_ADC_3,
    WICED_ADC_4,
    WICED_ADC_5,
    WICED_ADC_6,
    WICED_ADC_7,
    WICED_ADC_8,
    WICED_ADC_9,
    WICED_ADC_10,
    WICED_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    WICED_ADC_32BIT = 0x7FFFFFFF,
} wiced_adc_t;

// The datasheet says up to 3 USARTs, numbered 1, 2 and 6 for some reason...
typedef enum
{
    WICED_UART_1,
    WICED_UART_2,
	WICED_UART_6,
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    WICED_UART_32BIT = 0x7FFFFFFF,
} wiced_uart_t;

/******************************************************
 *                    Constants
 ******************************************************/

/* UART port used for standard I/O */
#define STDIO_UART ( WICED_UART_2 ) // WICED_UART_2 is STM32F411CE USART2 (defined in platform.c)
									// Connected to the USB-UART chip on the MXCHIP devboard

/* SPI flash is present on EMW3165 */
#define WICED_PLATFORM_INCLUDES_SPI_FLASH
#define WICED_SPI_FLASH_CS ( WICED_GPIO_5 )

/* Components connected to external I/Os */
#define WICED_LED1         ( WICED_GPIO_19 )	// According to schematic.
												// Not visible on module, at least with shield on
												// Need to have it defined for factory reset code to work
#define WICED_LED2         ( WICED_GPIO_19 )	// Same as LED1, due to snip.ota_fr depending on WICED_LED2

#define WICED_BUTTON1      ( WICED_GPIO_9 )		// Easylink button on dev board

#define WICED_SWITCH1      ( WICED_GPIO_36 )	// BOOT switch on dev board
#define WICED_SWITCH2      ( WICED_GPIO_37 )	// STATUS switch on dev board

#ifdef __cplusplus
} /*extern "C" */
#endif
