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

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define PWM_DEFAULT_VALUE 128

typedef enum
{ 
    PWM_CHANNEL_0 = 0,
    PWM_CHANNEL_1 = 1,
} PWM_CHANNEL;

typedef enum
{
    PWM_CONFIGURATION_DEFAULT
} PWM_CONFIGURATION;


void setPWM10bit(uint16_t value);

/*********************************************************************
* Function: bool PWM_Enable(PWM_CHANNEL channel, PWM_CONFIGURATION configuration);
*
* Overview: Enables specified channel
*
* PreCondition: none
*
* Input: PWM_CHANNEL channel - the channel to enable
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool PWM_Enable(PWM_CHANNEL channel);

/*********************************************************************
* Function: bool PWM_SetConfiguration(PWM_CONFIGURATION configuration)
*
* Overview: Configures the PWM module to specified setting
*
* PreCondition: none
*
* Input: PWM_CONFIGURATION configuration - the mode in which to run the PWM
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool PWM_SetConfiguration(PWM_CONFIGURATION configuration);

#endif	/* XC_HEADER_TEMPLATE_H */

