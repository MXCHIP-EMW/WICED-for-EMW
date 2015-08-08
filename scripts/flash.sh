#!/bin/bash
sudo openocd -f ./stlinkv2.cfg -c "init" -c "reset halt" -c "halt" -c "flash write_image erase ./WICED/build/waf_bootloader-NoOS-NoNS-EMW3162-SDIO/binary/waf_bootloader-NoOS-NoNS-EMW3162-SDIO.stripped.elf"  -c shutdown
sleep 1
sudo openocd -f ./stlinkv2.cfg -c "init" -c "reset halt" -c "halt" -c "flash write_image erase ./WICED/build/$1/DCT.stripped.elf" -c shutdown
sleep 1
sudo openocd -f ./stlinkv2.cfg -c "init" -c "reset halt" -c "halt" -c "flash write_image erase ./WICED/build/$1/binary/$1.stripped.elf" -c shutdown
sleep 1
sudo openocd -f ./stlinkv2.cfg -c "init" -c "reset" -c shutdown