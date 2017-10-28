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
#ifndef HID_H
#define HID_H

/** I N C L U D E S **********************************************************/
#include "typedefs.h"

/** D E F I N I T I O N S ****************************************************/

/* Class-Specific Requests */
#define GET_REPORT      0x01
#define GET_IDLE        0x02
#define GET_PROTOCOL    0x03
#define SET_REPORT      0x09
#define SET_IDLE        0x0A
#define SET_PROTOCOL    0x0B

/* Class Descriptor Types */
#define DSC_HID         0x21
#define DSC_RPT         0x22
#define DSC_PHY         0x23

/* Protocol Selection */
#define BOOT_PROTOCOL   0x00
#define RPT_PROTOCOL    0x01


/* HID Interface Class Code */
#define HID_INTF                    0x03

/* HID Interface Class SubClass Codes */
#define BOOT_INTF_SUBCLASS          0x01

/* HID Interface Class Protocol Codes */
#define HID_PROTOCOL_NONE           0x00
#define HID_PROTOCOL_KEYBOAD        0x01
#define HID_PROTOCOL_MOUSE          0x02

/******************************************************************************
 * Macro:           (bit) mHIDRxIsBusy(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This macro is used to check if HID OUT endpoint is
 *                  busy (owned by SIE) or not.
 *                  Typical Usage: if(mHIDRxIsBusy())
 *
 * Note:            None
 *****************************************************************************/
#define mHIDRxIsBusy()              HID_BD_OUT.Stat.UOWN

/******************************************************************************
 * Macro:           (bit) mHIDTxIsBusy(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This macro is used to check if HID IN endpoint is
 *                  busy (owned by SIE) or not.
 *                  Typical Usage: if(mHIDTxIsBusy())
 *
 * Note:            None
 *****************************************************************************/
#define mHIDTxIsBusy()              HID_BD_IN.Stat.UOWN

/******************************************************************************
 * Macro:           uint8_t mHIDGetRptRxLength(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          mHIDGetRptRxLength returns hid_rpt_rx_len
 *
 * Side Effects:    None
 *
 * Overview:        mHIDGetRptRxLength is used to retrieve the number of bytes
 *                  copied to user's buffer by the most recent call to
 *                  HIDRxReport function.
 *
 * Note:            None
 *****************************************************************************/
#define mHIDGetRptRxLength()        hid_rpt_rx_len


/* HID macros */
#define mUSBGetHIDDscAdr(ptr)               \
{                                           \
    if(usb_active_cfg == 1)                 \
        ptr = (ROM uint8_t*)&cfg01.hid_i00a00; \
}

#define mUSBGetHIDRptDscAdr(ptr)            \
{                                           \
    if(usb_active_cfg == 1)                 \
        ptr = (ROM uint8_t*)&hid_rpt01;        \
}

#define mUSBGetHIDRptDscSize(count)         \
{                                           \
    if(usb_active_cfg == 1)                 \
        count = sizeof(hid_rpt01);          \
}



/** S T R U C T U R E S ******************************************************/
typedef struct _USB_HID_DSC_HEADER
{
    uint8_t bDscType;
    uint16_t wDscLength;
} USB_HID_DSC_HEADER;

typedef struct _USB_HID_DSC
{
    uint8_t bLength;       uint8_t bDscType;      uint16_t bcdHID;
    uint8_t bCountryCode;  uint8_t bNumDsc;
    USB_HID_DSC_HEADER hid_dsc_header[HID_NUM_OF_DSC];
    /*
     * HID_NUM_OF_DSC is defined in autofiles\usb_config.h
     */
} USB_HID_DSC;

/** E X T E R N S ************************************************************/
extern uint8_t hid_rpt_rx_len;
extern ROM uint8_t hid_rpt01[HID_RPT01_SIZE];


/** P U B L I C  P R O T O T Y P E S *****************************************/
void HIDInitEP(void);
void USBCheckHIDRequest(void);
void HIDTxReport(char *buffer, uint8_t len);
uint8_t HIDRxReport(char *buffer, uint8_t len);

#endif //HID_H
