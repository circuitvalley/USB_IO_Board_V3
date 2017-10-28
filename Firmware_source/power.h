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

#ifndef BSP_POWER_H
#define BSP_POWER_H

/** Type defintions *********************************/
typedef enum
{
    POWER_SOURCE_USB,
    POWER_SOURCE_MAINS
} POWER_SOURCE;


/*********************************************************************
* Function: POWER_SOURCE POWER_SourceGet(void)
*
* Overview: Gets the current source of power for the board
*
* PreCondition: None
*
* Input: None
*
* Output: POWER_SOURCE - the current source of power for the board
*
********************************************************************/
POWER_SOURCE POWER_SourceGet(void);
#define POWER_SourceGet() POWER_SOURCE_MAINS

#endif //BSP_POWER_H
