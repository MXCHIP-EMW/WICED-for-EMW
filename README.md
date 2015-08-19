# WICED for MXCHIP EMW wireless ARM modules

[![Join the chat at https://gitter.im/MXCHIP-EMW/WICED-for-EMW](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/MXCHIP-EMW/WICED-for-EMW?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
*Support for EMW3162 and EMW3165*
*This is still a bit fidgety, at least with flashing on EMW3165*
*EMW3165 only has SWD interface, rather than full JTAG and work is being done to support that properly with the WICED flashing tools*

[![Join the chat at https://gitter.im/EMW3162/WICED_for_EMW3162](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/EMW3162/WICED_for_EMW3162?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Download WICED_SDK_3.3.1.7z.zip from Broadcom. It requires registration on their site.
Place the file in this directory and run `./extract-and-patch-WICED`
This will decompress and patch WICED.
Enter the WICED-SDK-3.3.1 directory and run `./make EMW<model>-LwIP-FreeRTOS-snip.apsta download run` to compile and flash via the green development EMW3162 board or add `JTAG=jlink` for Segger J-Link.

Run `./make EMW<model>-LwIP-FreeRTOS-snip.apsta` and then `./flash.sh EMW<model>-LwIP-FreeRTOS-snip_apsta` to flash via st-link-v2. Note how you have to replace a period to underscore, between the two commands.
You may need to hold down reset while starting the flashing process while using st-link-v2.

If you want to stop the bootloader from starting the application, hold down the WPS and EASYLINK buttons, while you push RESET.
This allows you to flash the device, even if you have repurposed the default JTAG pins in the application.

Just make sure that the JTAG pins are not initialized to alternative functions in the platform config, but in the application instead.

Documentation, schematics, libraries and datasheets, among other related things, can be found in the *docs_and_libraries* directory, in this repository.

There is also a gitter channel for the MXCHIP EMW modules at https://gitter.im/willemwouters/EMW3162_WICED and the original work for WICED for EMW3162 [here](https://github.com/EMW3162/EMW3162_WICED), first derivative [here](https://github.com/EMW3162/WICED_for_EMW3162).
When a second module started to be sold on Seeedstudio, we migrated to a more generic named organization and repositories.

You can get all the supported modules at Seeedstudio
* [EMW3165 - Cortex-M4 based WiFi SoC Module](http://www.seeedstudio.com/depot/EMW3165-CortexM4-based-WiFi-SoC-Module-p-2488.html)
* [EMWE - 3165 - A Development Board](http://www.seeedstudio.com/depot/EMWE-3165-A-Development-Board-p-2489.html)
* [EMW3162 WiFi Module](http://www.seeedstudio.com/depot/EMW3162-WiFi-Module-p-2122.html)
* [EMW3162 WiFi Module (External IPEX antenna)](http://www.seeedstudio.com/depot/EMW3162-WiFi-Module-External-IPEX-antenna-p-2235.html)
* [EMB-WICED-S EMW3162 Development Board for WICED (aka. green board) _(Recommended due to the built-in WICED compatible JTAG)_](http://www.seeedstudio.com/depot/EMBWICEDS-EMW3162-Development-Board-for-WICED-p-2335.html)
* [EMB-380-S2 Development Board (aka. black board) _(Requires external JTAG adapter like Segger J-Link (or equivalent) or ST-Link V2)_](http://www.seeedstudio.com/depot/EMB380S2-Development-Board-p-2146.html)
* [ST-Link V2 for STM8 STM32 interface programmer](http://www.seeedstudio.com/depot/STLink-V2-for-STM8-STM32-interface-programmer-p-2297.html)
* [FT2232H USB 2.0 Hi-Speed breakout board](http://www.seeedstudio.com/depot/FT2232H-USB-20-HiSpeed-breakout-board-p-737.html) - Can be used for UART and JTAG. Refer to the EMB-WICED-S EMW3162 development board schematics to see how. Only works for EMW3162, due to the EMW3165 only having SWD interface.
* [Breadboard friendly breakout board for EMW3165](http://www.emw3165.com/viewtopic.php?f=11&t=12)

Both EMW3162 development boards come with headers to solder on to an EMW3162. If you plan on using more than one EMW3162 with a development board, you need to source 2mm headers. The most common type of header is 1 mil or 2.54mm and those don't fit.
SeeedStudio did not carry those headers when I last ordered a dev board.
You will also require a couple of strips of regular headers to solder onto the development board as breakouts for the EMW3162 pins.
The EMW3165 has a 1mm pin pitch and is soldered directly to the development board. The breakout pins on the development board are 2mm, for some reason and are not very convenient, as regular DuPont connectors can't be side by side easily.

(Join us)[http://www.emw3165.com/] on a forum dedicated to the EMW3165.
![Sample EMW3162 setup](https://raw.githubusercontent.com/EMW3162/WICED_for_EMW3162/master/docs_and_libraries/green-dev-board-with-annotations.png)
