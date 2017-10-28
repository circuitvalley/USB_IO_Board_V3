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

#include <adc.h>
#include <pwm.h>

#define PIN_ANALOG    1
#define PIN_DIGITAL   0

#define PIN_INPUT     1
#define PIN_OUTPUT    0

/*********************************************************************
* Function: ADC_ReadPercentage(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the percentage 
*            of that conversions result (0-100%).
*
* PreCondition: channel is configured via the ADCConfigure() function
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
uint8_t ADC_ReadPercentage
    (ADC_CHANNEL channel) {
    uint8_t percent;

    switch(channel)
    {
        case ADC_CHANNEL_10:
            break;
        default:
            return 0xFF;
    }
    
    //A very crude percentage calculation
    percent = (ADC_Read10bit(channel) / 10);

    if(percent > 100)
    {
        percent = 100;
    }
    return percent;
}
uint16_t get_adc_value_with_pwm ( ADC_CHANNEL channel , uint16_t pwm_value) 
{
    
    uint16_t result;
    
    setPWM10bit( pwm_value );
    
    ADCON0bits.CHS = channel;

    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.GO_nDONE);     // Wait for conversion

    result = ADRESH;
    result <<=8;
    result |= ADRESL;

    return result;
    
    
}
/*********************************************************************
* Function: ADC_Read10bit(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the 10-bit
*           representation of this data.
*
* PreCondition: channel is configured via the ADCConfigure() function
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
uint16_t ADC_Read10bit(ADC_CHANNEL channel)
{
    uint16_t result;

    switch(channel)
    {
        case ADC_CHANNEL_10:
            break;
        default:
            return 0xFFFF;
    }

    ADCON0bits.CHS = channel;

    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.GO_nDONE);     // Wait for conversion

    result = ADRESH;
    result <<=8;
    result |= ADRESL;

    return result;
}

/*********************************************************************
* Function: bool ADC_Enable(ADC_CHANNEL channel, ADC_CONFIGURATION configuration);
*
* Overview: Configures the ADC module to specified setting
*
* PreCondition: none
*
* Input: ADC_CHANNEL channel - the channel to enable
*        ADC_CONFIGURATION configuration - the mode in which to run the ADC
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool ADC_Enable(ADC_CHANNEL channel)
{
    switch(channel)
    {
        case ADC_CHANNEL_10:
            TRISBbits.TRISB4 = PIN_INPUT;
            ANSELBbits.ANSB4 = PIN_ANALOG;
            return true;
      case ADC_CHANNEL_3:
            TRISAbits.TRISA4 = PIN_INPUT;
            ANSELAbits.ANSA4 = PIN_ANALOG;
            return true;

        default:
            return false;
    }
}

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
bool ADC_SetConfiguration(ADC_CONFIGURATION configuration)
{
    if(configuration == ADC_CONFIGURATION_DEFAULT)
    {
        FVRCON = 0x83; 
        ADCON0 = 0x29;
        ADCON1 = 0xE3; //internal reference set to 4.096 v
        ADCON2 = 0x00;
        
        return true;
    }

    return false;
}
