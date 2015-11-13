/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 * Defines STM32F4xx WICED application framework functions
 */
#include <string.h>
#include <stdlib.h>
#include "spi_flash.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "wwd_assert.h"
#include "wiced_framework.h"
#include "elf.h"
#include "wiced_apps_common.h"
#include "waf_platform.h"

#define PLATFORM_APP_START_SECTOR      ( FLASH_Sector_3  )
#define PLATFORM_APP_END_SECTOR        ( FLASH_Sector_7  )

#define APP_CODE_START_ADDR   ((uint32_t)&app_code_start_addr_loc)
#define SRAM_START_ADDR       ((uint32_t)&sram_start_addr_loc)
extern void* app_code_start_addr_loc;
extern void* sram_start_addr_loc;

#if defined( PLATFORM_STM32_VOLTAGE_1V8_TO_2V1 )
#define ERASE_VOLTAGE_RANGE ( VoltageRange_1 )
#define FLASH_WRITE_FUNC    ( FLASH_ProgramByte )
#define FLASH_WRITE_SIZE    ( 1 )
typedef uint8_t flash_write_t;
#elif defined( PLATFORM_STM32_VOLTAGE_2V1_TO_2V7 )
#define ERASE_VOLTAGE_RANGE ( VoltageRange_2 )
#define FLASH_WRITE_FUNC    ( FLASH_ProgramHalfWord )
#define FLASH_WRITE_SIZE    ( 2 )
typedef uint16_t flash_write_t;
#elif defined( PLATFORM_STM32_VOLTAGE_2V7_TO_3V6 )
#define ERASE_VOLTAGE_RANGE ( VoltageRange_3 )
#define FLASH_WRITE_FUNC    ( FLASH_ProgramWord )
#define FLASH_WRITE_SIZE    ( 4 )
typedef uint32_t flash_write_t;
#elif defined( PLATFORM_STM32_VOLTAGE_2V7_TO_3V6_EXTERNAL_VPP )
#define ERASE_VOLTAGE_RANGE ( VoltageRange_4 )
#define FLASH_WRITE_FUNC    ( FLASH_ProgramDoubleWord )
#define FLASH_WRITE_SIZE    ( 8 )
typedef uint64_t flash_write_t;
#else
/* No Voltage range defined for platform */
/* You need to define one of:
 *   PLATFORM_STM32_VOLTAGE_1V8_TO_2V1
 *   PLATFORM_STM32_VOLTAGE_2V1_TO_2V7
 *   PLATFORM_STM32_VOLTAGE_2V7_TO_3V6
 *   PLATFORM_STM32_VOLTAGE_2V7_TO_3V6_EXTERNAL_VPP
 */
#error Platform Voltage Range not defined
#endif

#if defined ( __ICCARM__ )

static inline void __jump_to( uint32_t addr )
{
    __asm( "MOV R1, #0x00000001" );
    __asm( "ORR R0, R1, #0" );  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
    __asm( "BX R0" );
}

#elif defined ( __GNUC__ )

__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
    addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __ASM volatile ("BX %0" : : "r" (addr) );
}

#endif

void platform_start_app( uint32_t entry_point )
{

    /* Simulate a reset for the app: */
    /*   Switch to Thread Mode, and the Main Stack Pointer */
    /*   Change the vector table offset address to point to the app vector table */
    /*   Set other registers to reset values (esp LR) */
    /*   Jump to the reset vector */


    if ( entry_point == 0 )
    {
        uint32_t* vector_table =  (uint32_t*) APP_CODE_START_ADDR;
        entry_point = vector_table[1];
    }


    __asm( "MOV LR,        #0xFFFFFFFF" );
    __asm( "MOV R1,        #0x01000000" );
    __asm( "MSR APSR_nzcvq,     R1" );
    __asm( "MOV R1,        #0x00000000" );
    __asm( "MSR PRIMASK,   R1" );
    __asm( "MSR FAULTMASK, R1" );
    __asm( "MSR BASEPRI,   R1" );
    __asm( "MSR CONTROL,   R1" );

/*  Now rely on the app crt0 to load VTOR / Stack pointer

    SCB->VTOR = vector_table_address; - Change the vector table to point to app vector table
    __set_MSP( *stack_ptr ); */

    __jump_to( entry_point );

}

platform_result_t platform_erase_flash( uint16_t start_sector, uint16_t end_sector )
{
    uint32_t i;

    /* Unlock the STM32 Flash */
    FLASH_Unlock( );

    /* Clear any error flags */
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );

    platform_watchdog_kick( );

    for ( i = start_sector; i <= end_sector; i += 8 )
    {
        if ( FLASH_EraseSector( i, ERASE_VOLTAGE_RANGE ) != FLASH_COMPLETE )
        {
            /* Error occurred during erase. */
            /* TODO: Handle error */
            while ( 1 )
            {
            }
        }
        platform_watchdog_kick( );
    }

    FLASH_Lock( );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_write_flash_chunk( uint32_t address, const void* data, uint32_t size )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint32_t write_address   = address;
    flash_write_t* data_ptr  = (flash_write_t*) data;
    flash_write_t* end_ptr   = (flash_write_t*) &((char*)data)[size];

    FLASH_Unlock( );

    /* Clear any error flags */
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );

    /* Write data to STM32 flash memory */
    while ( data_ptr <  end_ptr )
    {
        FLASH_Status status;

        if ( ( ( ((uint32_t)write_address) & 0x03 ) == 0 ) && ( end_ptr - data_ptr >= FLASH_WRITE_SIZE ) )
        {
            int tries = 0;
            /* enough data available to write as the largest size allowed by supply voltage */
            while ( ( FLASH_COMPLETE != ( status = FLASH_WRITE_FUNC( write_address, *data_ptr ) ) ) && ( tries < 10 ) )
            {
                tries++;
            }
            if ( FLASH_COMPLETE != status )
            {
                /* TODO: Handle error properly */
                wiced_assert("Error during write", 0 != 0 );
            }
            write_address += FLASH_WRITE_SIZE;
            data_ptr++;
        }
        else
        {
            int tries = 0;
            /* Limited data available - write in bytes */
            while ( ( FLASH_COMPLETE != ( status = FLASH_ProgramByte( write_address, (uint8_t) *data_ptr ) ) ) && ( tries < 10 ) )
            {
                tries++;
            }
            if ( FLASH_COMPLETE != status )
            {
                /* TODO: Handle error properly */
                wiced_assert("Error during write", 0 != 0 );
            }
            write_address++;
            data_ptr = (flash_write_t*)((uint32_t)data_ptr+1);
        }

    }
    if ( memcmp( (void*)address, (void*)data, size) != 0 )
    {
        result = PLATFORM_ERROR;
    }
    FLASH_Lock();

    return result;
}

void platform_erase_app_area( uint32_t physical_address, uint32_t size )
{
    /* if app in RAM, no need for erase */
    if ( physical_address < SRAM_START_ADDR )
    {
        if (physical_address == (uint32_t)DCT1_START_ADDR)
        {
            platform_erase_flash( PLATFORM_DCT_COPY1_START_SECTOR, PLATFORM_DCT_COPY1_END_SECTOR );
        }
        else
        {
            platform_erase_flash( PLATFORM_APP_START_SECTOR, PLATFORM_APP_END_SECTOR );
        }
    }
    (void) size;
}

/* The function would copy data from serial flash to internal flash.
 * The function assumes that the program area is already erased (for now).
 * TODO: Adding erasing the required area
 */
static wiced_result_t platform_copy_app_to_iflash( const image_location_t* app_header_location, uint32_t offset, uint32_t physical_address, uint32_t size )
{
    /* Bootloader doesn't support BSS sections. */
    uint8_t buff[ 64 ];

    while ( size > 0 )
    {
        uint32_t write_size = MIN( sizeof(buff), size);
        wiced_apps_read( app_header_location, buff, offset, write_size );
        platform_write_flash_chunk( (uint32_t) physical_address, buff, write_size );
        if (memcmp((char *)physical_address, buff, write_size))
        {
            offset = 0;
            return WICED_ERROR;
        }
        offset           += write_size;
        physical_address += write_size;
        size             -= write_size;
    }
    return WICED_SUCCESS;
}

void platform_load_app_chunk( const image_location_t* app_header_location, uint32_t offset, void* physical_address, uint32_t size )
{
    if ( (uint32_t) physical_address < SRAM_START_ADDR )
    {
        platform_copy_app_to_iflash( app_header_location, offset, (uint32_t) physical_address, size );
    }
    else
    {
        wiced_apps_read( app_header_location, physical_address, offset, size );
    }
}
