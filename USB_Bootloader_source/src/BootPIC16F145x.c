/*******************************************************************************
     
    File:   BootPIC16F145x.c
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

/** I N C L U D E S **********************************************************/
#include "usb.h"
#include "BootPIC16F145x.h"

/** C O N S T A N T S **********************************************************/
//The bootloader version, which the bootloader PC application can do extended query to get.
//Value provided is expected to be in the format of BOOTLOADER_VERSION_MAJOR.BOOTLOADER_VERSION_MINOR
//Ex: 1.01 would be BOOTLOADER_VERSION_MAJOR == 1, and BOOTLOADER_VERSION_MINOR == 1
#define BOOTLOADER_VERSION_MAJOR         1 //Legal value 0-255
#define BOOTLOADER_VERSION_MINOR         2 //Legal value 0-99.  (1 = X.01)


//Section defining the address range to erase for the erase device command, 
//along with the valid programming range to be reported by the QUERY_DEVICE command.


#if defined(_16F1459) || defined(_16F1455) || defined(_16F1454) || defined(_16LF1459) || defined(_16LF1455) || defined(_16LF1454)
    // PIC16F145x devices have 8192 Words of Flash
    // 32 words per block = 64 bytes per block
    // Bootloader ocupies aprox 3K Words of the flash
    // Range for this Bootloader code   = 0x000 to 0xAFF
    // Range for User Application code  = 0xB00 to 0x1FFF

    //PROGRAM_MEM_START_ADDRESS is the beginning of application program memory
    //(not occupied by bootloader).  To change this value, edit the BootPIC16F145x.h file.
    #define PROGRAM_MEM_START_ADDRESS   (APP_SPACE_START_ADDRESS * 2)  //**THIS VALUE MUST BE ALIGNED WITH AN ERASE PAGE BOUNDARY**
    #define USER_END                    0x1FFF  // Last location in USER FLASH
    #define MAX_PAGE_TO_ERASE           (USER_END / 32)   // Last 64 byte page of flash on the PIC16F145x
    #define PROGRAM_MEM_STOP_ADDRESS    ((USER_END+1) * 2)  //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**
    #define CONFIG_WORDS_START_ADDRESS  (uint32_t)(0x8007UL * 2)   //0x8000 is CONFIG space for PIC16F145x Family
    #define CONFIG_WORDS_SECTION_LENGTH (2 * 2)         //2 bytes worth of Configuration words on the PIC16F145x family
    #define USER_ID_ADDRESS             (uint32_t)(0x8000UL * 2)   //User ID is 3 bytes starting at 0x8000
    #define USER_ID_SIZE                (3 * 2)
    #define WRITE_BLOCK_SIZE            0x40            //64 byte programming block size (32 words) on the PIC16F145x family devices
    #define ERASE_PAGE_SIZE             0x40            //64 byte erase block size (32 words) on the PIC16F145x family devices
    #define ERASE_PAGE_NUM_WORDS        (ERASE_PAGE_SIZE / 2)
    #define ERASE_PAGE_ADDRESS_MASK     0xFFE0          //AND mask to move any flash address back to the start of the erase page

#endif


//Bootloader Command From Host - Switch() State Variable Choices
#define QUERY_DEVICE                0x02    //Command that the host uses to learn about the device (what regions can be programmed, and what type of memory is the region)
#define UNLOCK_CONFIG               0x03    //Note, this command is used for both locking and unlocking the config bits (see the "//Unlock Configs Command Definitions" below)
#define ERASE_DEVICE                0x04    //Host sends this command to start an erase operation.  Firmware controls which pages should be erased.
#define PROGRAM_DEVICE              0x05    //If host is going to send a full RequestDataBlockSize to be programmed, it uses this command.
#define PROGRAM_COMPLETE            0x06    //If host send less than a RequestDataBlockSize to be programmed, or if it wished to program whatever was left in the buffer, it uses this command.
#define GET_DATA                    0x07    //The host sends this command in order to read out memory from the device.  Used during verify (and read/export hex operations)
#define RESET_DEVICE                0x08    //Resets the microcontroller, so it can update the config bits (if they were programmed, and so as to leave the bootloader (and potentially go back into the main application)
#define SIGN_FLASH                  0x09    //The host PC application should send this command after the verify operation has completed successfully.  If checksums are used instead of a true verify (due to ALLOW_GET_DATA_COMMAND being commented), then the host PC application should send SIGN_FLASH command after is has verified the checksums are as exected. The firmware will then program the SIGNATURE_WORD into flash at the SIGNATURE_ADDRESS.
#define QUERY_EXTENDED_INFO         0x0C    //Used by host PC app to get additional info about the device, beyond the basic NVM layout provided by the query device command

//Unlock Configs Command Definitions
#define UNLOCKCONFIG                0x00    //Sub-command for the ERASE_DEVICE command
#define LOCKCONFIG                  0x01    //Sub-command for the ERASE_DEVICE command

//Query Device Response "Types" 
#define MEMORY_REGION_PROGRAM_MEM   0x01    //When the host sends a QUERY_DEVICE command, need to respond by populating a list of valid memory regions that exist in the device (and should be programmed)
#define MEMORY_REGION_EEDATA        0x02
#define MEMORY_REGION_CONFIG        0x03
#define MEMORY_REGION_USERID        0x04
#define MEMORY_REGION_END           0xFF    //Sort of serves as a "null terminator" like number, which denotes the end of the memory region list has been reached.
#define BOOTLOADER_V1_01_OR_NEWER_FLAG   0xA5   //Tacked on in the VersionFlag byte, to indicate when using newer version of bootloader with extended query info available


//BootState Variable States
#define IDLE                        0x00
#define NOT_IDLE                    0x01

//OtherConstants
#define INVALID_ADDRESS             0xFFFFFFFF
#define CORRECT_UNLOCK_KEY          0xB5

//Application and Microcontroller constants
#define BYTES_PER_ADDRESS_PIC16     0x01    //One byte per address.  PIC24 uses 2 bytes for each address in the hex file.
#define USB_PACKET_SIZE             0x40
#define WORDSIZE                    0x02    //PIC16/PIC18 uses 2 byte words, PIC24 uses 3 byte words.
#define REQUEST_DATA_BLOCK_SIZE     0x3A    //Number of data bytes in a standard request to the PC.  Must be an even number from 2-58 (0x02-0x3A).  Larger numbers make better use of USB bandwidth and 
                                            //yeild shorter program/verify times, but require more micrcontroller RAM for buffer space.
#define BLANK_FLASH_WORD_VALUE      0x3FFF

/** USB Packet Request/Response Formatting Structure **********************************************************/
typedef union 
{
    unsigned char Contents[USB_PACKET_SIZE];

    //General command (with data in it) packet structure used by PROGRAM_DEVICE and GET_DATA commands
    struct
    {
        unsigned char Command;
        unsigned long Address;      //Little endian (address LSB is Contents[1] in the array)
        unsigned char Size;
//          unsigned char PadBytes[58-REQUEST_DATA_BLOCK_SIZE]; //Uncomment this if using a smaller than 0x3A RequestDataBlockSize.  Compiler doesn't like 0 byte array when using 58 byte data block size.
            unsigned char Data[REQUEST_DATA_BLOCK_SIZE];
    };
        
    //This struct used for responding to QUERY_DEVICE command (on a device with four programmable sections)
    struct
    {
        unsigned char Command;
        unsigned char PacketDataFieldSize;
        unsigned char BytesPerAddress;
        unsigned char Type1;
        unsigned long Address1;
        unsigned long Length1;
        unsigned char Type2;
        unsigned long Address2;
        unsigned long Length2;
        unsigned char Type3;
        unsigned long Address3;
        unsigned long Length3;
        unsigned char Type4;
        unsigned long Address4;
        unsigned long Length4;
        unsigned char Type5;
        unsigned long Address5;
        unsigned long Length5;
        unsigned char Type6;
        unsigned long Address6;
        unsigned long Length6;
        unsigned char VersionFlag;      //Used by host software to identify if device is new enough to support QUERY_EXTENDED_INFO command  
        unsigned char ExtraPadBytes[7];
    };
        
    struct
    {                       //For UNLOCK_CONFIG command
        unsigned char Command;
        unsigned char LockValue;
    };

    //Structure for the QUERY_EXTENDED_INFO command (and response)
    struct{
        unsigned char Command;
        unsigned int BootloaderVersion;
        unsigned int ApplicationVersion;
        unsigned long SignatureAddress;
        unsigned int SignatureValue;
        unsigned long ErasePageSize;
        unsigned char Config1LMask;
        unsigned char Config1HMask;
        unsigned char Config2LMask;
        unsigned char Config2HMask;
        unsigned char Config3LMask;
        unsigned char Config3HMask;
        unsigned char Config4LMask;
        unsigned char Config4HMask;
        unsigned char Config5LMask;
        unsigned char Config5HMask;
        unsigned char Config6LMask;
        unsigned char Config6HMask;
        unsigned char Config7LMask;
        unsigned char Config7HMask;
    };          
} PacketToFromPC;
    

/** V A R I A B L E S ********************************************************/
//unsigned int  ProgramMemStopAddress;
unsigned char BootState;
unsigned int  ErasePageTracker;
unsigned char BufferedDataIndex;
unsigned int  ProgrammedPointer;
unsigned char ConfigsLockValue;
unsigned char ProgrammingBuffer[WRITE_BLOCK_SIZE];

PacketToFromPC PacketFromPC;
PacketToFromPC PacketToPC;


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserInit(void);
void WriteFlashBlock(void);
void WriteConfigBits(void);
void WriteEEPROM(void);
void UnlockAndActivate(unsigned char UnlockKey);
void ResetDeviceCleanly(void);
void SignFlash(void);
void LowVoltageCheck(void);


/** D E C L A R A T I O N S **************************************************/

void UserInit(void)
{
    //Initialize bootloader state variables
    BootState = IDLE;
    ProgrammedPointer = INVALID_ADDRESS;
    BufferedDataIndex = 0;
    ConfigsLockValue = TRUE;

}//end UserInit




/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function receives/sends USB packets to/from the USB 
 *                  host.  It also processes any received OUT packets and
 *                  is reponsible for generating USB IN packet data.
 *
 * Note:            None
 *****************************************************************************/
void ProcessIO(void)
{
    unsigned char i;
    unsigned long int TmpAddr;

    //Checks for and processes application related USB packets (assuming the
    //USB bus is in the CONFIGURED_STATE, which is the only state where
    //the host is allowed to send application related USB packets to the device.
    if((USBGetDeviceState() != CONFIGURED_STATE) || (USBIsDeviceSuspended() == 1))
    {
        //No point to trying to run the application code until the device has
        //been configured (finished with enumeration) and is not currently suspended.
        return;
    }

    //Check the current bootloader state (if we are currently waiting from a new
    //command to process from the host, or if we are still processing a previous
    //command.
    if(BootState == IDLE)
    {
        //We are currently in the IDLE state waiting for a command from the
        //PC software on the USB host.
        if(!mHIDRxIsBusy()) //Did we receive a command?
        {
            //We received a new command from the host.  Copy the OUT packet from 
            //the host into a local buffer for processing.
            HIDRxReport((char *)&PacketFromPC, USB_PACKET_SIZE);     //Also re-arms the OUT endpoint to be able to receive the next packet
            BootState = NOT_IDLE;   //Set flag letting state machine know it has a command that needs processing.
            
            //Pre-initialize a response packet buffer (only used for some commands)
            for(i = 0; i < USB_PACKET_SIZE; i++)        //Prepare the next packet we will send to the host, by initializing the entire packet to 0x00.
                PacketToPC.Contents[i] = 0;             //This saves code space, since we don't have to do it independently in the QUERY_DEVICE and GET_DATA cases.
        }
    }//if(BootState == IDLE)
    else //(BootState must be NOT_IDLE)
    {   
        //Check the latest command we received from the PC app, to determine what
        //we should be doing.
        switch(PacketFromPC.Command)
        {
            case QUERY_DEVICE:
                //Make sure the USB IN endpoint buffer is available, then load
                //up a query response packet to send to the host.
                if(!mHIDTxIsBusy())
                {
                    //Prepare a response packet, which lets the PC software know about the memory ranges of this device.
                    PacketToPC.Command = QUERY_DEVICE;
                    PacketToPC.PacketDataFieldSize = REQUEST_DATA_BLOCK_SIZE;
                    PacketToPC.BytesPerAddress = BYTES_PER_ADDRESS_PIC16;
                    PacketToPC.Type1 = MEMORY_REGION_PROGRAM_MEM;
                    PacketToPC.Address1 = (unsigned long)PROGRAM_MEM_START_ADDRESS;
                    PacketToPC.Length1 = (unsigned long)(PROGRAM_MEM_STOP_ADDRESS - PROGRAM_MEM_START_ADDRESS); //Size of program memory area
                    PacketToPC.Type2 = MEMORY_REGION_CONFIG;
                    PacketToPC.Address2 = (unsigned long)CONFIG_WORDS_START_ADDRESS;
                    PacketToPC.Length2 = (unsigned long)CONFIG_WORDS_SECTION_LENGTH;
                    PacketToPC.Type3 = MEMORY_REGION_USERID;        //Not really program memory (User ID), but may be treated as it it was as far as the host is concerned
                    PacketToPC.Address3 = (unsigned long)USER_ID_ADDRESS;
                    PacketToPC.Length3 = (unsigned long)(USER_ID_SIZE);
                    PacketToPC.Type4 = MEMORY_REGION_END;
                    #if defined(DEVICE_WITH_EEPROM)
                        PacketToPC.Type4 = MEMORY_REGION_EEDATA;
                        PacketToPC.Address4 = (unsigned long)EEPROM_EFFECTIVE_ADDRESS;
                        PacketToPC.Length4 = (unsigned long)EEPROM_SIZE;
                        PacketToPC.Type5 = MEMORY_REGION_END;
                    #endif
                    PacketToPC.VersionFlag = BOOTLOADER_V1_01_OR_NEWER_FLAG; //To let host PC GUI program know that we are a v1.01 or newer device
                    //Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxReport()).
    
                    //Now send the packet to the USB host software, assuming the USB endpoint is available/ready to accept new data.
                    HIDTxReport((char *)&PacketToPC, USB_PACKET_SIZE);
                    BootState = IDLE;
                }
                break;

            case UNLOCK_CONFIG:
                ConfigsLockValue = TRUE;
                if(PacketFromPC.LockValue == UNLOCKCONFIG)
                {
                    ConfigsLockValue = FALSE;
                }
                BootState = IDLE;
                break;

            case ERASE_DEVICE:
                //First erase main program flash memory
                for(ErasePageTracker = APP_SPACE_START_ADDRESS; ErasePageTracker < USER_END; ErasePageTracker += ERASE_PAGE_NUM_WORDS)
                {
                    ClrWdt();
                    PMADR = ErasePageTracker;
                    CFGS = 0;  // Access FLASH space not CONFIG
                    FREE = 1;  // Perform erase on next WR command, cleared by HW
                    UnlockAndActivate(CORRECT_UNLOCK_KEY);
                }
                
                //Now erase the User ID space (0x8000 to 0x8008)
                PMADR = 0;
                CFGS = 1;   // Config space
                FREE = 1;
                UnlockAndActivate(CORRECT_UNLOCK_KEY);

                BootState = IDLE;               
                break;

            case PROGRAM_DEVICE:
                //Check if host is trying to program the User ID bytes (or config bits, which are at an even higher address)
                if(PacketFromPC.Address >= USER_ID_ADDRESS)
                {     
                    //Check if the host is trying to program the config bits
                    if(PacketFromPC.Address >= CONFIG_WORDS_START_ADDRESS)
                    {
                        //Check if the host correctly send the unlock config command.  If not,
                        //ignore the write request and do not change the config bit values.
                        if(ConfigsLockValue == FALSE)
                        {
                            WriteConfigBits();      //Doesn't get reprogrammed if the UNLOCK_CONFIG (LockValue = UNLOCKCONFIG) command hasn't previously been sent
                        }
                    }
                    else
                    {
                        //Just writing the User ID bytes
                        WriteConfigBits();
                    }
                    BootState = IDLE;
                    break;
                }

                if(ProgrammedPointer == (unsigned int)INVALID_ADDRESS)
                    ProgrammedPointer = PacketFromPC.Address;
                
                if(ProgrammedPointer == (unsigned int)PacketFromPC.Address)
                {
                    for(i = 0; i < PacketFromPC.Size; i++)
                    {
                        ProgrammingBuffer[BufferedDataIndex] = PacketFromPC.Data[i+(REQUEST_DATA_BLOCK_SIZE-PacketFromPC.Size)];    //Data field is right justified.  Need to put it in the buffer left justified.
                        BufferedDataIndex++;
                        ProgrammedPointer++;
                        if(BufferedDataIndex == WRITE_BLOCK_SIZE)
                        {
                            WriteFlashBlock();
                        }
                    }
                }
                //else host sent us a non-contiguous packet address...  to make 
                //this firmware simpler, host should not do this without sending 
                //a PROGRAM_COMPLETE command in between program sections.
                BootState = IDLE;
                break;

            case PROGRAM_COMPLETE:
                WriteFlashBlock();
                ProgrammedPointer = INVALID_ADDRESS;        //Reinitialize pointer to an invalid range, so we know the next PROGRAM_DEVICE will be the start address of a contiguous section.
                BootState = IDLE;
                break;

            case GET_DATA:
                //Assuming the USB IN (to host) buffer is available/ready, prepare a packet to send to the host
                if(!mHIDTxIsBusy())
                {
                    //Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxReport()).
                    PacketToPC.Command = GET_DATA;
                    PacketToPC.Address = PacketFromPC.Address;
                    PacketToPC.Size = PacketFromPC.Size;

                    TmpAddr = ((uint32_t)PacketFromPC.Address / 2);
                    PMADR = TmpAddr;

                    //Read every byte of flash memory that the PC app is requesting
                    for(i = 0; i < PacketFromPC.Size; i++)
                    {
                        if(TmpAddr >= CONFIG_WORDS_START_ADDRESS)
                        {
                            PMCON1bits.CFGS = 1;   // Read from config not Flash
                            PMADRH=0;
                        }
                        else
                        {
                            PMCON1bits.CFGS = 0;   // Read from Flash not config
                        }

                        PMCON1bits.RD = 1;     // Initiate Read
                        NOP();               // Two instruction delay in read
                        NOP();

                        PacketToPC.Data[i+((USB_PACKET_SIZE - 6) - PacketFromPC.Size)] = PMDATL;  // Low byte first in the data
                        i++;

                        //Check if we should exit from for() loop early, in case PC
                        //GUI app is requesting an odd number of bytes from the device.
                        if(i >= PacketFromPC.Size)
                            break;

                        //Check if the read 14-bit WORD from flash is blank or not (0x3FFF is blank value)
                        if(PMDAT != BLANK_FLASH_WORD_VALUE)
                        {
                            //The word was not blank, return the real high byte info from the flash
                            PacketToPC.Data[i+((USB_PACKET_SIZE - 6) - PacketFromPC.Size)] = PMDATH; // regular return
                        }
                        else
                        {
                            //The 14-bit flash word was blank.  In this case, return 0xFF in the high byte,
                            //instead of 0x3F, since the PC GUI app is assuming a PIC18 style device where
                            //all blank bytes will read as 0xFF, when performing the verify comparison.
                            PacketToPC.Data[i+((USB_PACKET_SIZE - 6) - PacketFromPC.Size)] = 0xFF;  // Faked return
                        }
                        PMADR++; // Next address
                    }//for(i = 0; i < PacketFromPC.Size; i++)

                    //Now arm the USB IN endpoint to send the packet to the host.
                    HIDTxReport((char *)&PacketToPC, USB_PACKET_SIZE);
                    BootState = IDLE;
                }//if(!HIDTxHandleBusy(USBInHandle)) //if(!mHIDTxIsBusy())
                break;

            case SIGN_FLASH:
                SignFlash();
                BootState = IDLE;
                break;
            case QUERY_EXTENDED_INFO:
                //Prepare a response packet with the QUERY_EXTENDED_INFO response info in it.
                //This command is only supported in bootloader firmware verison 1.01 or later.
                //Make sure the regular QUERY_DEVIER reponse packet value "PacketToPC.Type6" is = BOOTLOADER_V1_01_OR_NEWER_FLAG;
                //to let the host PC software know that the QUERY_EXTENDED_INFO command is implemented
                //in this firmware and is available for requesting by the host software.
                PacketToPC.Command = QUERY_EXTENDED_INFO;   //Echo the command byte
                PacketToPC.BootloaderVersion = ((unsigned int)BOOTLOADER_VERSION_MAJOR << 8)| BOOTLOADER_VERSION_MINOR;
                PacketToPC.ApplicationVersion = *(ROM unsigned int*)APP_VERSION_ADDRESS;
                PacketToPC.SignatureAddress = (APP_SIGNATURE_ADDRESS * 2);  //*2 is to convert 14-bit word into a byte address like the .hex file uses
                PacketToPC.SignatureValue = ((uint16_t)0x3400) | APP_SIGNATURE_VALUE;
                PacketToPC.ErasePageSize = ERASE_PAGE_SIZE;
                PacketToPC.Config1LMask = 0xFF;
                PacketToPC.Config1HMask = 0xFF;
                PacketToPC.Config2LMask = 0xFF;
                PacketToPC.Config2HMask = 0xFF;
                PacketToPC.Config3LMask = 0xFF;
                PacketToPC.Config3HMask = 0xFF;
                PacketToPC.Config4LMask = 0xFF;
                PacketToPC.Config4HMask = 0xFF;
                PacketToPC.Config5LMask = 0xFF;
                PacketToPC.Config5HMask = 0xFF;
                PacketToPC.Config6LMask = 0xFF;
                PacketToPC.Config6HMask = 0xFF;
                PacketToPC.Config7LMask = 0xFF;
                PacketToPC.Config7HMask = 0xFF;
                
                //Now actually command USB to send the packet to the host                   
                if(!mHIDTxIsBusy())
                {
                    HIDTxReport((char *)&PacketToPC, USB_PACKET_SIZE);
                    BootState = IDLE;   //Packet will be sent, go back to idle state ready for next command from host
                }       
                break;
            case RESET_DEVICE:
                ResetDeviceCleanly();
                //break;    //no need, commented to save space
            default:
                //Should never hit the default
                BootState = IDLE;
                
        }//End switch

    }//End of else of if(BootState == IDLE)
}//End ProcessIO()


//Should be called once, only after the regular erase/program/verify sequence 
//has completed successfully.  This function will program the magic
//APP_SIGNATURE_VALUE into the magic APP_SIGNATURE_ADDRESS in the application
//flash memory space.  This is used on the next bootup to know that the the
//flash memory image of the application is intact, and can be executed.
//This is useful for recovery purposes, in the event that an unexpected
//failure occurs during the erase/program sequence (ex: power loss or user
//unplugging the USB cable).
void SignFlash(void)
{
    static unsigned char i;

    //First read in the erase page contents of the page with the signature WORD
    //in it, and temporarily store it in a RAM buffer.
    PMADR = (uint16_t)(APP_SIGNATURE_ADDRESS & ERASE_PAGE_ADDRESS_MASK);
    PMCON1bits.CFGS = 0;
    for(i = 0; i < ERASE_PAGE_SIZE; i)
    {
        PMCON1bits.RD = 1;  //Initiate flash memory read operation
        Nop();              //2 Nops() required, see datasheet
        Nop();
        ProgrammingBuffer[i++] = PMDATL;
        ProgrammingBuffer[i++] = PMDATH;
        PMADR++;
    }

    //Now change the signature WORD value at the correct address in the RAM buffer
    ProgrammingBuffer[(APP_SIGNATURE_ADDRESS & ~ERASE_PAGE_ADDRESS_MASK) * 2] = (unsigned char)APP_SIGNATURE_VALUE;
    ProgrammingBuffer[((APP_SIGNATURE_ADDRESS & ~ERASE_PAGE_ADDRESS_MASK) * 2) + 1] = 0x34;   //RETLW opcode = 0x34XX (where XX is the WREG literal value returned)

    //Now erase the flash memory block with the signature WORD in it
    ClrWdt();
    PMADR = APP_SIGNATURE_ADDRESS;
    CFGS = 0;  // Access FLASH space not CONFIG
    FREE = 1;  // Perform erase on next WR command, cleared by HW
    UnlockAndActivate(CORRECT_UNLOCK_KEY);

    //Now re-program the values from the RAM buffer into the flash memory.  Use
    //reverse order, so we program the larger addresses first.  This way, the
    //write page with the flash signature word is the last page that gets
    //programmed (assuming the flash signature resides on the lowest address
    //write page, which is recommended, so that it becomes the first page
    //erased, and the last page programmed).
    PMCON1bits.WREN = 1;
    CFGS = 0;   // Access Prog not config
    FREE = 0;   // Flash Write Mode (i.e. Not Erase)
    PMADR = (uint16_t)(APP_SIGNATURE_ADDRESS & ERASE_PAGE_ADDRESS_MASK);
    PMCON1bits.LWLO = 1;    //Load latches only for now

    //Write all the program memory write latches
    for(i = 0; i < ERASE_PAGE_SIZE; i)
    {
        //Check if this the last word to write or not, if so, clear LWLO so
        //the unlock sequence initiates the write operation
        if(i == (WRITE_BLOCK_SIZE - 2))
        {
            PMCON1bits.LWLO = 0;
        }

        //Prepare to write the low byte to the program latch
        PMDATL = ProgrammingBuffer[i++];
        //Prepare to write the high byte to the program latch
        PMDATH = ProgrammingBuffer[i++];
        //Commit the data to the write latches with unlock sequence
        UnlockAndActivate(CORRECT_UNLOCK_KEY);
        //Increment flash word pointer
        PMADR++;
    }

    //Good practice now to fully disable any further erase/write operations
    PMCON1bits.LWLO = 1;
    PMCON1bits.WREN = 0;
    
}    


//Before resetting the microcontroller, we should shut down the USB module 
//gracefully, to make sure the host correctly recognizes that we detached
//from the bus.  Some USB hosts malfunction/fail to re-enumerate the device
//correctly if the USB device does not stay detached for a minimum amount of
//time before re-attaching to the USB bus.  For reliable operation, the USB
//device should stay detached for as long as a human would require to unplug and
//reattach a USB device (ex: 100ms+), to ensure the USB host software has a 
//chance to process the detach event and configure itself for a state ready for 
//a new attachment event.
void ResetDeviceCleanly(void)
{
    USBDisableWithLongDelay();
    Reset();    
    Nop();
    Nop();
}





//Routine used to write data to the flash memory from the ProgrammingBuffer[].
void WriteFlashBlock(void)      //Use to write blocks of data to flash.
{
    static unsigned char i;
    static unsigned char BytesTakenFromBuffer;
    static unsigned char CorrectionFactor;
    unsigned int Addr;

    BytesTakenFromBuffer = 0;

    Addr = (ProgrammedPointer - BufferedDataIndex) >> 1;    //Convert byte address to 14-bit word address

    //Do error check to make sure the address to be programmed is in range
    if((Addr < APP_SPACE_START_ADDRESS) || (Addr > USER_END))return;


    //Check the lower 5 bits of the TBLPTR to verify it is pointing to a 32 byte aligned block (5 LSb = 00000).
    //If it isn't, need to somehow make it so before doing the actual loading of the programming latches.
    //In order to maximize programming speed, the PC application meant to be used with this firmware will not send
    //large blocks of 0xFF bytes.  If the PC application
    //detects a large block of unprogrammed space in the hex file (effectively = 0xFF), it will skip over that
    //section and will not send it to the firmware.  This works, because the firmware will have already done an
    //erase on that section of memory when it received the ERASE_DEVICE command from the PC.  Therefore, the section
    //can be left unprogrammed (after an erase the flash ends up = 0xFF).
    //This can result in a problem however, in that the next genuine non-0xFF section in the hex file may not start
    //on a 32 byte aligned block boundary.  This needs to be handled with care since the microcontroller can only
    //program 32 byte blocks that are aligned with 32 byte boundaries.
    //So, use the below code to avoid this potential issue.

    PMADR = Addr;

    CorrectionFactor = (PMADRL & 0b00011111);   //Correctionfactor = number of WORDS tblptr must go back to find the immediate preceeding 64 byte boundary
    PMADRL &= 0b11100000;           //Move the table pointer back to the immediately preceeding 32 WORD boundary

    for(i = 0; i < (WRITE_BLOCK_SIZE / 2); i++) //Load the programming latches
    {
        CFGS = 0;   // Access Prog not config
        FREE = 0;   // Flash Write Mode (i.e. Not Erase)
        if(i == ((WRITE_BLOCK_SIZE / 2) - 1))
        {
            LWLO = 0;    // Write the page
        }
        else
        {
            LWLO = 1;   // Write to latches only
        }

        if(CorrectionFactor == 0)
        {
            if(BufferedDataIndex != 0)  //If the buffer isn't empty
            {
                PMDATL = ProgrammingBuffer[BytesTakenFromBuffer];
                BytesTakenFromBuffer++;
                BufferedDataIndex--;    //Used up a byte from the buffer.

                PMDATH = ProgrammingBuffer[BytesTakenFromBuffer];
                BytesTakenFromBuffer++;
                BufferedDataIndex--;    //Used up a byte from the buffer.
            }
            else    //No more data in buffer, need to write 0xFF to fill the rest of the programming latch locations
            {
                PMDAT = 0x3FFF;
            }
        }
        else
        {
            PMDAT = 0x3FFF;
            CorrectionFactor--;
        }

     
        UnlockAndActivate(CORRECT_UNLOCK_KEY);

        PMADR++;   // Move to Next buffer

    }

    //Now need to fix the ProgrammingBuffer[].  We may not have taken a full 64 bytes out of the buffer.  In this case,
    //the data is no longer justified correctly.
    for(i = 0; i < BufferedDataIndex; i++)  //Need to rejustify the remaining data to the "left" of the buffer (if there is any left)
    {
        ProgrammingBuffer[i] = ProgrammingBuffer[BytesTakenFromBuffer+i];
    }
}


void WriteConfigBits(void)  //Also used to write the Device ID
{
    static unsigned char i,j;

    PMADRL = (unsigned int)PacketFromPC.Address >> 1;
    PMADRH = 0; //Clear High for config access

    j = (PacketFromPC.Size - 2);

    for(i = 0; i < PacketFromPC.Size; i++)
    {
        if(i == j)
        {
            LWLO = 0;    // Write the page
        }
        else
        {
            LWLO = 1;   // Write to latches only
        }

        PMDATL = PacketFromPC.Data[i + (REQUEST_DATA_BLOCK_SIZE-PacketFromPC.Size)];
        i++;
        PMDATH = PacketFromPC.Data[i + (REQUEST_DATA_BLOCK_SIZE-PacketFromPC.Size)];

        CFGS = 1;    // Access Config
        FREE = 0;
        LWLO = 0;    // Write the page

        UnlockAndActivate(CORRECT_UNLOCK_KEY);

        PMADR++;
    }
}


//It is preferrable to only place this sequence in only one place in the flash memory.
//This reduces the probabilty of the code getting executed inadvertently by
//errant code.  It is also recommended to enable BOR (in hardware) and/or add
//software checks to avoid microcontroller "overclocking".  Always make sure
//to obey the voltage versus frequency graph in the datasheet, even during
//momentary events (such as the power up and power down ramp of the microcontroller).
void UnlockAndActivate(unsigned char UnlockKey)
{
    INTCONbits.GIE = 0;     //Make certain interrupts disabled for unlock process.

    //Make sure voltage is sufficient for safe self erase/write operations
    LowVoltageCheck();

    //Check to make sure the caller really was trying to call this function.
    //If they were, they should always pass us the CORRECT_UNLOCK_KEY.
    if(UnlockKey != CORRECT_UNLOCK_KEY)
    {
        //Warning!  Errant code execution detected.  Somehow this 
        //UnlockAndActivate() function got called by someone that wasn't trying
        //to actually perform an NVM erase or write.  This could happen due to
        //microcontroller overclocking (or undervolting for an otherwise allowed
        //CPU frequency), or due to buggy code (ex: incorrect use of function 
        //pointers, etc.).  In either case, we should execute some fail safe 
        //code here to prevent corruption of the NVM contents.
        OSCCON = 0x03;  //Switch to INTOSC at low frequency
        while(1)
        {
            Sleep();
        }    
        Reset();
    }

    //Make sure watchdog recently cleared.  Programming operations can take awhile...
    ClrWdt();

    // Do Write
    WREN = 1;        //Enable write/erase operations
    // Execute the unlock sequence
    PMCON2 = 0x55;
    PMCON2 = 0xAA;
    WR=1;
    asm("NOP");
    asm("NOP");
    WREN = 0;        //Good practice to keep WREN clear when not needed
    // End Do Write
}   





/** EOF BootPIC16F145x.c *********************************************************/
