/*******************************************************************************
     
    File:   main.c
    Author:  Gaurav Singh
    website: www.circuitvalley.com 
    Created on October 28, 2017
    
    This file is part of Circuitvalley USB HID Bootloader.

    Circuitvalley USB HID Bootloader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Circuitvalley USB HID Bootloader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Circuitvalley USB HID Bootloader.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef __HARDWARE_PROFILE_H_
#define __HARDWARE_PROFILE_H_


// PIC16F145x device have an internal oscillator that can be used with USB.
// uncomment the next line to use the internal oscillator with USB. Make sure
// to add at least 1uF of capacitance on VDD net if using the Low Pin Count
// USB Development Kit board with board number starting with 02-02043.
// If you are using the newer version of the board (02-10255), no changes are needed.
//#define USE_INTERNAL_OSC


//Make sure board/platform specific definitions (like config bit settings and
//I/O pin definitions are correct for your hardware platform).
#if defined(__16F1459)|| defined (__16F1454)
    #define LOW_PIN_COUNT_USB_DEVELOPMENT_KIT
#else
    #define YOUR_CUSTOM_BOARD
    #advisory "You need to add platform specific settings for your hardware.  Double click this message for more details."
    //In order to use a hardware platform other than a Microchip USB demo board, you need to make
    //sure the following are correctly configured for your hardware platform:
    //1. Configuration bit settings (especially the oscillator settings, which must be compatible with USB operation).
    //2. I/O pin definitions for VBUS sensing, self power sensing, I/O pushbutton for entry into bootloader, and for LED blink settings.
    //3. Optional behavioral settings: ENABLE_IO_PIN_CHECK_BOOTLOADER_ENTRY, ENABLE_USB_LED_BLINK_STATUS, USE_SELF_POWER_SENSE_IO, USE_USB_BUS_SENSE_IO.  See usb_config.h file.
    //4. Oscillator and other settings are correctly being initialized in the InitializeSystem() function, specific to your hardware (ex: turn on PLL [if needed] for proper USB clock, etc.)
#endif


#if defined(LOW_PIN_COUNT_USB_DEVELOPMENT_KIT)   //Based on PIC16F1459
    //VBUS sensing pin definition, applicable if using the USE_USB_BUS_SENSE_IO option in usb_config.
    #if defined(USE_USB_BUS_SENSE_IO)
        #define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
        #define usb_bus_sense       PORTAbits.RA1
    #endif
    #if defined(USE_SELF_POWER_SENSE_IO)
        #define tris_self_power     TRISXbits.TRISXX	//Replace with real value if your hardware supports this feature
        #define self_power          PORTXbits.RXX       //Replace with real value if your hardware supports this feature
    #endif

    //LED definition, applicable if using ENABLE_USB_LED_BLINK_STATUS option in usb_config.h
    #define mLED1       LATCbits.LATC0
    #define mLED1Tris   TRISCbits.TRISC0
    /** SWITCH *********************************************************/
    #define mInitSwitch2()      {}                      //No TRISA3 bit.  RA3 is input only on this device
    #define sw2                 PORTAbits.RA3           //Requires MCLR disabled to use RA3 as general purpose input
    #define mDeInitSwitch2()    {} 



#elif defined(YOUR_CUSTOM_BOARD)
    #advisory "Edit your hardware specific I/O pin mapping values here."
    //Modify the below template values to be appropriate for your hardware platform.

    //VBUS sensing pin definition, applicable if using the USE_USB_BUS_SENSE_IO option in usb_config.
    #if defined(USE_USB_BUS_SENSE_IO)
        #define tris_usb_bus_sense  TRISXbits.TRISXX    // Input
        #define usb_bus_sense       PORTXbits.RXX
    #endif
    #if defined(USE_SELF_POWER_SENSE_IO)
        #define tris_self_power     TRISXbits.TRISXX	//Replace with real value if your hardware supports this feature
        #define self_power          PORTXbits.RXX       //Replace with real value if your hardware supports this feature
    #endif

    //LED definition, applicable if using ENABLE_USB_LED_BLINK_STATUS option in usb_config.h
    #define mLED1       DummyVar    //If using an LED, replace with LAT bit (ex: LATCbits.LATC0)
    #define mLED1Tris   DummyVar    //If using an LED, replace with TRIS bit (ex: TRISCbits.TRISC0)
    /** SWITCH *********************************************************/
    #define mInitSwitch2()      {}                      //No TRISA3 bit.  RA3 is input only on this device
    #define sw2                 PORTAbits.RA3           //Requires MCLR disabled to use RA3 as general purpose input
    #define mDeInitSwitch2()    {}

#else
    #error Not a supported board (yet), add I/O pin mapping in __FILE__, line __LINE__
#endif
    /** I/O pin definitions ********************************************/
    #define INPUT_PIN 1
    #define OUTPUT_PIN 0

#endif //__HARDWARE_PROFILE_H_
