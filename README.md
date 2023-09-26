#                      usb-winusb_with_stm32

## Basic info

  "Readme.md" is a good idea and convenient to be read by the users, but as a lazy guy, is not convenient to me .
so that ... see in code comment !!!

​    If you use your stm32 or some other mcu as a winusb devices (GUID_DEVINTERFACE_USB_DEVICE in Device Description ),here is the basic transmission and receive functions to connect your device to your windows PC.

It's just an easy proj and as a newbie, & wish your Sincere advices.

As said an easy proj, it haven't hot-plug event callback. if needed, u could add it yourself or call me(if available).

## Attention

Alright,remember to change the PID&VID and endpoints in file "usb_serial.cpp",then you can use class "USBSerial" as a parent class or a member of u class.

