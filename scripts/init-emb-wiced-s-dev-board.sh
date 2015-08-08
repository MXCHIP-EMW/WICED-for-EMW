#!/bin/bash
rmmod ftdi_sio 2>/dev/null
rmmod usbserial 2>/dev/null
sleep 1
modprobe ftdi_sio
modprobe usbserial
sleep 1
echo "0a5c 43fa" >> /sys/bus/usb-serial/drivers/ftdi_sio/new_id
