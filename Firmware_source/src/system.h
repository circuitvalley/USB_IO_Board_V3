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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <xc.h>
#include <stdbool.h>


#include "adc.h"

#include "io_mapping.h"
#include "fixed_address_memory.h"



//Internal oscillator option setting.  Uncomment if using HFINTOSC+active clock 
//tuning, instead of a crystal.  
//#define USE_INTERNAL_OSC        //Make sure 1uF-8uF extra capacitance is added on VDD net
                                //to smooth VDD ripple from MAX3232 chip, before using this
                                //with the original Low Pin Count USB Development Kit board.
                                //If using the latest version of the board, this is not
                                //required and is already present.



#define MAIN_RETURN void

/*** System States **************************************************/
typedef enum
{
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

/*********************************************************************
* Function: void SYSTEM_Initialize( SYSTEM_STATE state )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  SYSTEM_STATE - the state to initialize the system into
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize( SYSTEM_STATE state );


/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
//void SYSTEM_Tasks(void);
#define SYSTEM_Tasks()

#endif //SYSTEM_H
