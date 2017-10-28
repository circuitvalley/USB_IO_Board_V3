/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/
#ifndef USB_H
#define USB_H

/*
 * usb.h provides a centralize way to include all files
 * required by Microchip USB Firmware.
 *
 * The order of inclusion is important.
 * Dependency conflicts are resolved by the correct ordering.
 */

#include "typedefs.h"
#include "usb_config.h"
#include "usb_device.h"
#include "HardwareProfile.h"

#if defined(USB_USE_HID)                // See usb_config.h
#include "usb_device_hid.h"
#endif


//These callback functions belong in your main.c (or equivalent) file.  The USB
//stack will call these callback functions in response to specific USB bus events,
//such as entry into USB suspend mode, exit from USB suspend mode, and upon
//receiving the "set configuration" control tranfer request, which marks the end
//of the USB enumeration sequence and the start of normal application run mode (and
//where application related variables and endpoints may need to get (re)-initialized.
void USBCBSuspend(void);
void USBCBWakeFromSuspend(void);
void USBCBInitEP(uint8_t ConfigurationIndex);
void USBCBCheckOtherReq(void);


//API renaming wrapper functions
#define HIDTxHandleBusy(a)   {mHIDTxIsBusy()}
#define HIDRxHandleBusy(a)   {mHIDRxIsBusy()}

#endif //USB_H
