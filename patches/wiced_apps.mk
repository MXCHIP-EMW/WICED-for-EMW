#
# Copyright 2014, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

SECTOR_COUNT_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_count.pl
SECTOR_ADDRESS_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_address.pl

APPS_SECTORS_DEFAULT_COUNT	:= 1
CURRENT_SECTOR := $(APPS_START_SECTOR)

APPS_HEADER_DEFINES :=
CURRENT_DEPENDENCY :=
SFLASH_APP_TARGET := waf.sflash_write-NoOS-NoNS-$(PLATFORM_FULL)-$(BUS)

OPENOCD_LOG_FILE ?= build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

###############################################################################
# MACRO: BUILD_RESOURCE_RULES
# Creates targets to build a resource file
# the first target converts the text resource file to a C file
# the second target compiles the C resource file into an object file
# $(1) is the name of a resource
# $(2) should be MEM or FILESYSTEM - indication location of resource
define BUILD_APPS_RULES
$(if $($(1)),$(eval $(1)_ENTRY_COUNT := 1),$(eval $(1)_ENTRY_COUNT := 0))
$(if $($(1)),$(eval $(1)_SECTOR_START := $(CURRENT_SECTOR)),$(eval $(1)_SECTOR_START := 0))
$(if $($(1)),$(eval $(1)_SECTOR_COUNT := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $($(1)) 0 4096)),$(eval $(1)_SECTOR_COUNT := 0))
$(if $($(1)),$(eval $(1)_SECTOR_ADDRESS := $(shell $(PERL) $(SECTOR_ADDRESS_SCRIPT) $($(1)_SECTOR_START) 4096)),)
$(if $($(1)),$(eval CURRENT_SECTOR := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $($(1)) $(CURRENT_SECTOR) 4096)),)
$(eval APPS_HEADER_DEFINES += -D$(1)_ENTRY_COUNT=$($(1)_ENTRY_COUNT))
$(eval APPS_HEADER_DEFINES += -D$(1)_SECTOR_START=$($(1)_SECTOR_START))
$(eval APPS_HEADER_DEFINES += -D$(1)_SECTOR_COUNT=$($(1)_SECTOR_COUNT))
endef

define BUILD_APP_DOWNLOAD_DEPENDENCY
$(if $($(1)),$(eval $(1)_DOWNLOAD_DEPEDENCY := $($(1)) $(CURRENT_DEPENDENCY) sflash_write_app display_map_summary $(APPS_LUT_DOWNLOAD_DEP) APPS_LOOKUP_TABLE_RULES $(LINK_APPS_FILE)),)
$(if $($(1)),$(eval CURRENT_DEPENDENCY += $(1)_DOWNLOAD),)
endef

APPS_DOWNLOADS_DEPENDENCY :=
APPS := FR_APP DCT_IMAGE OTA_APP FILESYSTEM_IMAGE WIFI_FIRMWARE APP0 APP1 APP2
$(foreach APP,$(APPS),$(eval $(if $($(APP)), APPS_HEADER_DEPENDENCY += $($(APP)))))
$(foreach APP,$(APPS),$(eval $(if $($(APP)), APPS_DOWNLOADS_DEPENDENCY += $(APP)_DOWNLOAD)))
$(foreach APP,$(APPS),$(eval $(call BUILD_APP_DOWNLOAD_DEPENDENCY,$(APP))))

LINK_APPS_FILE            :=$(OUTPUT_DIR)/APPS$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_APPS_FILE   :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_APPS_FILE           :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
MAP_APPS_FILE             :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=.map)

.PHONY: DOWNLOAD_APPS_HEADER FILESYSTEM_IMAGE_DOWNLOAD APP0_DOWNLOAD APPS_LOOKUP_TABLE_RULES

APPS_LOOKUP_TABLE_RULES: display_map_summary $(APPS_HEADER_DEPENDENCY)
	$(foreach APP,$(APPS),$(eval $(call BUILD_APPS_RULES,$(APP))))

$(LINK_APPS_FILE): display_map_summary $(SOURCE_ROOT)WICED/platform/MCU/wiced_apps_lut.c APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Building apps lookup table
	$(QUIET)$(CC) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG)  $(SOURCE_ROOT)WICED/platform/MCU/wiced_apps_lut.c $(APPS_HEADER_DEFINES) $(WICED_SDK_DEFINES) $(WICED_SDK_INCLUDES) $(COMPILER_SPECIFIC_DEBUG_CFLAGS)  $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS, ) -I$(OUTPUT_DIR) -I$(SOURCE_ROOT). -o $(OUTPUT_DIR)/apps_header.o $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
	$(QUIET)$(LINKER) $(WICED_SDK_LDFLAGS) $(WICED_SDK_DCT_LINK_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_APPS_FILE)) -o $@  $(OUTPUT_DIR)/apps_header.o $(COMPILER_SPECIFIC_STDOUT_REDIRECT)

$(STRIPPED_LINK_APPS_FILE): $(LINK_APPS_FILE)
	$(QUIET)$(STRIP) -o $@ $(STRIPFLAGS) $<

$(FINAL_APPS_FILE): $(STRIPPED_LINK_APPS_FILE)
	$(QUIET)$(OBJCOPY) -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes $< $@

#define BUILD_APPS_DOWNLOAD_RULES
#$(1)_DOWNLOAD: $($(1)) $(APPS_DOWNLOAD_SERIALIZE) $(FS_DEP) display_map_summary download_dct APPS_LOOKUP_TABLE_RULES
#	$(foreach APP,$(APPS),$(eval $(call BUILD_APPS_RULES,$(APP))))
#	$(QUIET)$(ECHO) Downloading $(1)... at $($(1)_SECTOR_ADDRES)
#	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $($(1)) $($(1)_SECTOR_ADDRESS) $(subst /,_,$(PLATFORM_FULL)-$(BUS)) 0 1" -c shutdown #$(DOWNLOAD_LOG) 2>&1
#	$(QUIET)$(ECHO) Done...

#	This is to avoid parallel download to sflash
#	$(if $($(1)),$(eval APPS_DOWNLOAD_SERIALIZE += $(1)_DOWNLOAD),)
#endef
#$(foreach APP,$(APPS),$(eval $(call BUILD_APPS_DOWNLOAD_RULES,$(APP))))

# THIS IS A DODGY HACK AS THE DEFINE MACRO IS NOT WORKING!!!
ifneq ($(FR_APP),)
FR_APP_DOWNLOAD:  $(FR_APP_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading FR_APP $(FR_APP) at sector $(FR_APP_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FR_APP) $(FR_APP_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(OTA_APP),)
OTA_APP_DOWNLOAD:  $(OTA_APP_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading OTA_APP $(OTA_APP) at sector $(OTA_APP_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA_APP) $(OTA_APP_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(DCT_IMAGE),)
DCT_IMAGE_DOWNLOAD:  $(DCT_IMAGE_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading DCT_IMAGE $(DCT_IMAGE) at sector $(DCT_IMAGE_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(DCT_IMAGE) $(DCT_IMAGE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(FILESYSTEM_IMAGE),)
FILESYSTEM_IMAGE_DOWNLOAD: $(FILESYSTEM_IMAGE_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading resources filesystem $(FILESYSTEM_IMAGE) at sector $(FILESYSTEM_IMAGE_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FILESYSTEM_IMAGE) $(FILESYSTEM_IMAGE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(WIFI_FIRMWARE),)
WIFI_FIRMWARE_DOWNLOAD: $(WIFI_FIRMWARE_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading WIFI_FIRMWARE $(WIFI_FIRMWARE) at sector $(WIFI_FIRMWARE_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(WIFI_FIRMWARE) $(WIFI_FIRMWARE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP0),)
APP0_DOWNLOAD:  $(APP0_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading APP0 $(APP0) at sector $(APP0_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP0) $(APP0_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP1),)
APP1_DOWNLOAD:  $(APP1_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading APP1 $(APP1) at sector $(APP1_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP1) $(APP1_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP2),)
APP0_DOWNLOAD:  $(APP2_DOWNLOAD_DEPEDENCY)
	$(QUIET)$(ECHO) Downloading APP2 $(APP2) at sector $(APP2_SECTOR_START)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP2) $(APP2_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1
endif

# If Downloading is required, then the Serial Flash app need to be built
sflash_write_app:
	$(QUIET)$(ECHO) Building Serial Flash Loader App
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(SFLASH_APP_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=sflash_app $(SFLASH_REDIRECT)
	$(QUIET)$(ECHO) Finished Building Serial Flash Loader App
	$(QUIET)$(ECHO_BLANK_LINE)

APPS_LUT_DOWNLOAD: sflash_write_app $(FINAL_APPS_FILE) $(APPS_DOWNLOADS_DEPENDENCY) APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Downloading apps lookup table at $(APPS_LUT_HEADER_LOC)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FINAL_APPS_FILE) $(APPS_LUT_HEADER_LOC) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown #$(DOWNLOAD_LOG) 2>&1