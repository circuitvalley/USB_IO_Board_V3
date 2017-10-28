/*******************************************************************************

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

/*********************************************************************
* Function: void APP_LEDUpdateUSBStatus(void);
*
* Overview: Uses one LED to indicate the status of the device on the USB bus.
*           A fast blink indicates successfully connected.  A slow pulse
*           indicates that it is still in the process of connecting.  Off
*           indicates thta it is not attached to the bus or the bus is suspended.
*           This should be called on every start of frame packet reception and
*           if a suspend/resume event occurs.
*
* PreCondition: LEDs are enabled.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_LEDUpdateUSBStatus(void);
