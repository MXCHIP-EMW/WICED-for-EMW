# WICED for MXCHIP EMW wireless ARM modules
*Supports EMW3162 and EMW3165*

*The MXCHIP-EMW Github organization entity is not run by or affiliated with MXCHIP. Use sales@mxchip.com to contact MXCHIP*

[![Join the chat at https://gitter.im/MXCHIP-EMW/WICED-for-EMW](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/MXCHIP-EMW/WICED-for-EMW) === Active chat channel for the EMW community.

[Join us](http://www.emw3165.com) on a forum dedicated to the EMW3165 (and EMW3162). This forum is  administered by @jmg5150 and is neither affiliated by the MXCHIP-EMW organization nor by MXCHIP.

* Download WICED_SDK_3.3.1.7z.zip from Broadcom. It requires registration on their site **with what Broadcom calls a "corporate" e-mail address, so you can't use GMail, Outlook.com or other such e-mail addresses**.
* Place the file in this directory and run `./extract-and-patch-WICED`. This will decompress and patch WICED.
* Enter the WICED-SDK-3.3.1 directory and run something like `./make EMW<module no>-LwIP-FreeRTOS-<app-dir>.<app-name> download run JTAG=<jtag-adapter>` to compile and flash.

Step by step:
* Change into the SDK directory with `cd WICED-SDK-3.3.1`
* Test flashing an application to the module.

For EMW3165, using stlink-v2 for flashing, using the application *apsta* from the *snip* directory run:
`./make EMW3165-FreeRTOS-LwIP-snip.apsta download run JTAG=stlink-v2`

For EMW3165, using jlink, but otherwise as above:
`./make EMW3165-FreeRTOS-LwIP-snip.apsta download run JTAG=jlink`

For EMW3162, using the green development board w/FT2232H chip for flashing, but otherwise as above:
`./make EMW3162-FreeRTOS-LwIP-snip.apsta download run`

You may need to hold down reset while starting the flashing process while using st-link-v2.

* If the step above is successful, you should be able to see output via UART and see a wifi access point called `WICED Soft AP` with the password `abcd1234`.
* You can now start playing around. I recommend looking into apps/snip/apsta to play around. WICED comes with loads of sample application, so, look around, hack around and make stuff happen.
* Join us on Gitter for a chat if you experience any issues getting up and running.

Make sure that the JTAG pins are not initialized to alternative functions unless you are ready to find an alternative to be able to flash the module again. Nb. There is a built in bootloader in the STM32 family of MCUs, that allows flashing over, among other things, UART. Refer to datasheets to find out how that works (or ask on the gitter channel).

Some documentation, schematics (though EMW3165 schematics are not available), libraries and datasheets, among other related things, can be found in the *docs_and_libraries* directory, in this repository. There is also a subforum on emw3165.com dedicated to documentation.

The official Broadcom WICED site is a great resource for all things WICED. Search their forum and check out their blogs for answers to most of your WICED questions. Instructions on setting up Eclipse and debugging can be found on their blog.

You can get all the supported modules at Seeedstudio
* [EMW3165 - Cortex-M4 based WiFi SoC Module](http://www.seeedstudio.com/depot/EMW3165-CortexM4-based-WiFi-SoC-Module-p-2488.html)
* [EMWE - 3165 - A Development Board _(Requires external JTAG adapter like ST-Link V2, Segger J-Link or equivalent)_](http://www.seeedstudio.com/depot/EMWE-3165-A-Development-Board-p-2489.html)
* [EMW3162 WiFi Module](http://www.seeedstudio.com/depot/EMW3162-WiFi-Module-p-2122.html)
* [EMW3162 WiFi Module (External IPEX antenna)](http://www.seeedstudio.com/depot/EMW3162-WiFi-Module-External-IPEX-antenna-p-2235.html)
* [EMB-WICED-S EMW3162 Development Board for WICED (aka. green board) _(Recommended due to the built-in WICED compatible JTAG)_](http://www.seeedstudio.com/depot/EMBWICEDS-EMW3162-Development-Board-for-WICED-p-2335.html)
* [EMB-380-S2 Development Board (aka. black board) _(Requires external JTAG adapter like ST-Link V2, Segger J-Link or equivalent)_](http://www.seeedstudio.com/depot/EMB380S2-Development-Board-p-2146.html)
* [ST-Link V2 for STM8 STM32 interface programmer - _Best supported programmer for this project and it's cheap_](http://www.seeedstudio.com/depot/STLink-V2-for-STM8-STM32-interface-programmer-p-2297.html)
* [Breadboard friendly breakout board for EMW3165](http://www.emw3165.com/viewtopic.php?f=11&t=12)

Both EMW3162 development boards come with headers to solder on to an EMW3162. If you plan on using more than one EMW3162 with a development board, you need to source 2mm headers. The most common type of header are 2.54mm and those don't fit.
SeeedStudio did not carry those headers when I last ordered a dev board.
You will also require a couple of strips of regular headers to solder onto the development board as breakouts for the EMW3162 pins.
The EMW3165 has a 1mm pin pitch and is soldered directly to the development board. The breakout pins on the development board are 2mm, for some reason and are not very convenient, as regular dupont connectors can't be side by side easily.

![Sample EMW3162 setup](https://raw.githubusercontent.com/MXCHIP-EMW/WICED-for-EMW/master/docs_and_libraries/green-dev-board-with-annotations.png)

