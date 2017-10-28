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

#include <adc.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#include <pwm.h>

void setPWM10bit(uint16_t value)
{
    value = value & 0x3FF;
 
    PWM1DCL = value << 6;
    PWM1DCH = value >> 2;
}

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
bool PWM_Enable(PWM_CHANNEL channel)
{

    T2CONbits.TMR2ON = 1;
    PWM1CONbits.PWM1OE = 1;
    PWM1CONbits.PWM1EN = 1;
    
    return true;
}

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
bool PWM_SetConfiguration(PWM_CONFIGURATION configuration)
{
   if(configuration == PWM_CONFIGURATION_DEFAULT)
    {
        PWM1CON = 0; //active high , PWMx pin is disabled , PWM module disabled
        PWM1DCL = 0;
        PWM1DCH = 0x20;
        TRISCbits.TRISC5 = 0;
        
        T2CON = 0; //timer off Prescaler 1 , Postscaler 1
        PR2 = 255;
        
        
        return true;
    }
   return false;
}
