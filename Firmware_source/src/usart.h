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

#ifndef USART_H
#define USART_H

#include <stdbool.h>

#define CLOCK_FREQ 48000000
#define GetSystemClock() CLOCK_FREQ

#define UART_ENABLE   RCSTAbits.SPEN

#define UART_TRISTx   TRISBbits.TRISB7
#define UART_TRISRx   TRISBbits.TRISB5
#define UART_Tx       PORTBbits.RB7
#define UART_Rx       PORTBbits.RB5

// Use following only for Hardware Flow Control
#define UART_DTS PORTAbits.RA3
#define UART_DTR LATCbits.LATC3
#define UART_RTS LATBbits.LATB4
#define UART_CTS PORTBbits.RB6

#define mInitRTSPin() {TRISBbits.TRISB4 = 0;}   //Configure RTS as a digital output.
#define mInitCTSPin() {TRISBbits.TRISB6 = 1;}   //Configure CTS as a digital input.  (Make sure pin is digital if ANxx functions is present on the pin)
#define mInitDTSPin() {}//{TRISAbits.TRISA3 = 1;}   //Configure DTS as a digital input.  (Make sure pin is digital if ANxx functions is present on the pin)
#define mInitDTRPin() {TRISCbits.TRISC3 = 0;}   //Configure DTR as a digital output.


/*********************************************************************
* Function: void USART_Initialize(void);
*
* Overview: Initializes USART (RS-232 port)
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void USART_Initialize();

/******************************************************************************
 * Function:        void USART_putcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           char c - character to print to the UART
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
void USART_putcUSART(char);

/******************************************************************************
 * Function:        void USART_mySetLineCodingHandler(void)
 *
 * PreCondition:    USB_CDC_SET_LINE_CODING_HANDLER mySetLineCodingHandler is defined
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function gets called when a SetLineCoding command
 *                  is sent on the bus.  This function will evaluate the request
 *                  and determine if the application should update the baudrate
 *                  or not.
 *
 * Note:
 *
 *****************************************************************************/
void USART_mySetLineCodingHandler(void);

/******************************************************************************
 * Function:        unsigned char USART_getcUSART()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          unsigned char c - character to received on the UART
 *
 * Side Effects:    None
 *
 * Overview:        Get the input character from the UART
 *
 * Note:
 *
 *****************************************************************************/
unsigned char USART_getcUSART(void);

#endif //USART_H
