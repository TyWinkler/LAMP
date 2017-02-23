//*****************************************************************************
//
// Application Name     - SDCard.c
// Application Overview - Allows for easy function calls to be used with FileIO with an SDCard
// Application Details  - https://github.com/TyWinkler/LAMP/tree/master/modules/SDCard
// Authors              - Ty Winkler
//
//*****************************************************************************

//*****************************************************************************
// Pinmux config as follows
//
//// Configure PIN_06 for SDHOST0 SDHost_D0
//MAP_PinTypeSDHost(PIN_06, PIN_MODE_8);
//
//// Configure PIN_07 for SDHOST0 SDHost_CLK
//MAP_PinTypeSDHost(PIN_07, PIN_MODE_8);
//
//// Configure PIN_08 for SDHOST0 SDHost_CMD
//MAP_PinTypeSDHost(PIN_08, PIN_MODE_8);
//
//*****************************************************************************

// Hardware & driverlib library includes
#include <string.h>
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "hw_ints.h"
#include "common.h"
#include "uart_if.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "ff.h"
#include "sdhost.h"
#include "pin.h"
#include "prcm.h"

// Global Variables
FIL fp;
FATFS fs;
FRESULT res;
DIR dir;
UINT Size;

#define BUFFSIZE 100
char pBuffer[BUFFSIZE];
char File[];
char Directory[] = "/";

//*****************************************************************************
//
//! SD Card Initialization
//!
//! This function configures Pull up resistors for the SDCard Reader and sets
//! up the MMCHS peripherals.
//!
//! \return None.
//
//*****************************************************************************
void sdCardInit(){
    MAP_PinDirModeSet(PIN_07,PIN_DIR_MODE_OUT);                 // Set the SD card clock as output pin
    MAP_PinConfigSet(PIN_06,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on data
    MAP_PinConfigSet(PIN_08,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on CMD
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST,PRCM_RUN_MODE_CLK); // Enable MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST);                       // Reset MMCHS
    MAP_SDHostInit(SDHOST_BASE);                                // Configure MMCHS
    MAP_SDHostSetExpClk(SDHOST_BASE,MAP_PRCMPeripheralClockGet(PRCM_SDHOST),15000000); // Configure card clock
}

void setFile(char file[]){
    strcpy(File, file);
}

void setDirectoryPath(char directoryPath[]){
    strcpy(Directory, directoryPath);
}

void openDirectory(){
    res = f_opendir(&dir,Directory);
    if( res == FR_OK){
        Message("Opening root directory.................... [ok]\n\n\r");
        Message("/\n\r");
    } else {
        Message("Opening root directory.................... [Failed]\n\n\r");
        return;
    }
}

void ListDirectory(){
    FILINFO fno;
    FRESULT res;
    unsigned long ulSize;
    tBoolean bIsInKB;
    for(;;){
        res = f_readdir(&dir, &fno);           // Read a directory item
        if (res != FR_OK || fno.fname[0] == 0){
            break;                             // Break on error or end of dir
        }
        ulSize = fno.fsize;
        bIsInKB = false;
        if(ulSize > 1024){
            ulSize = ulSize/1024;
            bIsInKB = true;
        }
        Report("->%-15s%5d %-2s    %-5s\n\r",fno.fname,ulSize, (bIsInKB == true)?"KB":"B",(fno.fattrib&AM_DIR)?"Dir":"File");
    }
}

void mountFile(){
    f_mount(&fs,"0",1);
}

void openFile(){
    Message("\n\rOpening user file...\n\r");
    res = f_open(&fp,File,FA_READ);
}

void removeWavHeader(){
    f_lseek(&fp,44);
}

char* readFile(){
    if(res == FR_OK){
        f_read(&fp,pBuffer,BUFFSIZE,&Size);
        Report("Read : %d Bytes\n\n\r",Size);
        Report("%s",pBuffer);
    } else {
        Report("Failed to open %s\n\r",File);
    }
    return pBuffer;
}

void closeFile(){
    f_close(&fp);
}

void createFile(char fileName[], char *fileText){
    res = f_open(&fp,fileName,FA_CREATE_ALWAYS|FA_WRITE);
    if(res == FR_OK){
        f_write(&fp,fileText,sizeof(fileText),&Size);
        Report("Wrote : %d Bytes",Size);
        res = f_close(&fp);
    } else {
        Message("Failed to create a new file\n\r");
    }
}

UINT getSize(){
    return Size;
}

