NAME := Platform_EMW3162

WLAN_CHIP            := 43362
WLAN_CHIP_REVISION   := A2
WLAN_CHIP_FAMILY     := 43362
HOST_MCU_FAMILY      := STM32F2xx
HOST_MCU_VARIANT     := STM32F2x5
HOST_MCU_PART_NUMBER := STM32F205RGT6

INTERNAL_MEMORY_RESOURCES = $(ALL_RESOURCES)

ifndef BUS
BUS := SDIO
endif

VALID_BUSES := SDIO

EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/standard_platform_targets.mk

WIFI_IMAGE_DOWNLOAD := direct
GLOBAL_DEFINES      += WWD_DIRECT_RESOURCES

# Global includes
GLOBAL_INCLUDES  := .

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += HSE_VALUE=26000000
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)

# Components
# There is no SPI flash on EMW3162, but leaving this out makes WICED not compile
$(NAME)_COMPONENTS += drivers/spi_flash

# Source files
$(NAME)_SOURCES := platform.c

# WICED APPS
# APP0 and FILESYSTEM_IMAGE are reserved main app and resources file system
# FR_APP :=
# DCT_IMAGE :=
# OTA_APP :=
# FILESYSTEM_IMAGE :=
# WIFI_FIRMWARE :=
# APP0 :=
# APP1 :=
# APP2 :=

# WICED APPS LOOKUP TABLE
APPS_LUT_HEADER_LOC := 0x0000
APPS_START_SECTOR := 1

ifneq ($(APP),bootloader)
ifneq ($(MAIN_COMPONENT_PROCESSING),1)
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | IMPORTANT NOTES                                                                                     | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | Wi-Fi MAC Address                                                                                   | )
$(info |    The target Wi-Fi MAC address is defined in <WICED-SDK>/generated_mac_address.txt                 | )
$(info |    Ensure each target device has a unique address.                                                  | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | MCU & Wi-Fi Power Save                                                                              | )
$(info |    It is *critical* that applications using WICED Powersave API functions connect an accurate 32kHz | )
$(info |    reference clock to the sleep clock input pin of the WLAN chip. Please read the WICED Powersave   | )
$(info |    Application Note located in the documentation directory if you plan to use powersave features.   | )
$(info +-----------------------------------------------------------------------------------------------------+ )
endif
endif
