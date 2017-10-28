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


#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

/*** ADC Channel Definitions *****************************************/
#define ADC_CHANNEL_INPUT ADC_CHANNEL_3

typedef enum
{ 
    ADC_CHANNEL_10 = 10,
    ADC_CHANNEL_3 = 3,
} ADC_CHANNEL;

typedef enum
{
    ADC_CONFIGURATION_DEFAULT
} ADC_CONFIGURATION;

/*********************************************************************
* Function: ADC_ReadPercentage(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the percentage
*            of that conversions result (0-100%).
*
* PreCondition: channel is enabled via ADC_Enable()
*
* Input: ADC_CHANNEL channel - enumeration of the ADC channels
*        available in this demo.  They should be meaningful names and
*        not the names of the ADC pins on the device (as the demo code
*        may be ported to other boards).
*         i.e. ADC_ReadPercentage(ADC_CHANNEL_POTENTIOMETER);
*
* Output: uint8_t indicating the percentage of the result 0-100% or
*         0xFF for an error
*
********************************************************************/
uint8_t ADC_ReadPercentage(ADC_CHANNEL channel);
/*********************************************************************
* Function: get_adc_value_with_pwm ( ADC_CHANNEL channel , uint16_t pwm_value);
*
* Overview: Reads the requested ADC channel with PWM output set at pwm_value and returns the 10-bit
*           representation of this data.
*
* PreCondition: channel is enabled via ADC_Enable() , PWM is enabled 
*
* Input: ADC_CHANNEL channel - enumeration of the ADC channels
*        PWM_VALUE  - PWM 10bit value between 0 to 0x1FFF
*         i.e. - get_adc_value_with_pwm(ADC_CHANNEL_POTENTIOMETER,PWM_VALUE);
*
* Output: uint16_t the right adjusted 10-bit representation of the ADC
*         channel conversion or 0xFFFF for an error.
*
********************************************************************/

uint16_t get_adc_value_with_pwm ( ADC_CHANNEL channel , uint16_t pwm_value); 

/*********************************************************************
* Function: ADC_Read10bit(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the 10-bit
*           representation of this data.
*
* PreCondition: channel is enabled via ADC_Enable()
*
* Input: ADC_CHANNEL channel - enumeration of the ADC channels
*        available in this demo.  They should be meaningful names and
*        not the names of the ADC pins on the device (as the demo code
*        may be ported to other boards).
*         i.e. - ADCReadPercentage(ADC_CHANNEL_POTENTIOMETER);
*
* Output: uint16_t the right adjusted 10-bit representation of the ADC
*         channel conversion or 0xFFFF for an error.
*
********************************************************************/
uint16_t ADC_Read10bit(ADC_CHANNEL channel);

/*********************************************************************
* Function: bool ADC_Enable(ADC_CHANNEL channel, ADC_CONFIGURATION configuration);
*
* Overview: Enables specified channel
*
* PreCondition: none
*
* Input: ADC_CHANNEL channel - the channel to enable
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool ADC_Enable(ADC_CHANNEL channel);

/*********************************************************************
* Function: bool ADC_SetConfiguration(ADC_CONFIGURATION configuration)
*
* Overview: Configures the ADC module to specified setting
*
* PreCondition: none
*
* Input: ADC_CONFIGURATION configuration - the mode in which to run the ADC
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool ADC_SetConfiguration(ADC_CONFIGURATION configuration);

#endif  //ADC_H
