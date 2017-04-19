//*****************************************************************************
// speaker.c
//
// LINE OUT (Speaker Operation)
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
// Hardware & driverlib library includes
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "hw_ints.h"

// simplelink include
#include "simplelink.h"

// common interface includes
#include "common.h"
#include "uart_if.h"
// Demo app includes
#include "circ_buff.h"

//SDCard
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "ff.h"

#include "LPD8806.h"
#include "LCD.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
int g_iReceiveCount =0;
int g_iRetVal =0;
int iCount =0;
//unsigned char *p;
int songCount = 0;

#define USERFILE1        "call.wav"
#define USERFILE2        "stuck.wav"
#define BUFFSIZE                1024
#define PLAY_WATERMARK          50*1024
unsigned char pBuffer[BUFFSIZE];
unsigned char g_ucSpkrStartFlag = 0;
unsigned char songChanged = 0;

FIL fp;
extern FATFS fs;
extern FRESULT res;
extern DIR dir;
UINT Size;
char songname[30] = "call.wav";
//const char* myWav = songname;

extern unsigned long  g_ulStatus;
extern unsigned char g_uiPlayWaterMark;
extern unsigned char g_loopback;
//unsigned char speaker_data[16*1024];
extern tCircularBuffer *pRxBuffer;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

static void
ListDirectory()
{
    FILINFO fno;
    FRESULT res;
    unsigned long ulSize;
    tBoolean bIsInKB;

    for(;;)
    {
        res = f_readdir(&dir, &fno);           // Read a directory item
        if (res != FR_OK || fno.fname[0] == 0)
        {
        break;                                // Break on error or end of dir
        }
        ulSize = fno.fsize;
        bIsInKB = false;
        if(ulSize > 1024)
        {
            ulSize = ulSize/1024;
            bIsInKB = true;
        }
        Report("->%-15s%5d %-2s    %-5s\n\r",fno.fname,ulSize,\
                (bIsInKB == true)?"KB":"B",(fno.fattrib&AM_DIR)?"Dir":"File");
    }

}

FRESULT openFile(){
    Message("\n\rReading user file...\n\r");
    res = f_open(&fp,(TCHAR*)songname,FA_READ);
    f_lseek(&fp,44);
    return res;
}

void closeFile(){
    f_close(&fp);
}

void readFile(){
    if(songChanged){
#ifdef DEBUG
        LcdPrintf("Song Changed");
#endif
        closeFile();
        openFile();
        songChanged = 0;
    }
    if(res == FR_OK)
    {
        f_read(&fp,pBuffer,BUFFSIZE,&Size);
        //clearScreen();
    }
    else
    {
#ifdef DEBUG
        LcdPrintf("Didn't Read speaker");
#endif
        Report("Failed to open %s\n\r",USERFILE1);
    }
}



//*****************************************************************************
//
//! Speaker Routine 
//!
//! \param pvParameters     Parameters to the task's entry function
//!
//! \return None
//
//*****************************************************************************
void Speaker( void *pvParameters )
{
    long iRetVal;
    strcpy(songname,"Shake.wav");

//    ListDirectory();
    //open file
    openFile();
    g_ucSpkrStartFlag = 1;
    while(1)
    {
      while(g_ucSpkrStartFlag != 0)
      {
#ifdef DEBUG
          //clearScreen();
          //LcdPrintf("playing...");
#endif
        // Read from file and discard wav header
        readFile();
        /* Wait to avoid buffer overflow as reading speed is faster than playback */
        while((IsBufferSizeFilled(pRxBuffer,PLAY_WATERMARK) == TRUE)){
            osi_Sleep(10);
        }

        if( Size > 0)
        {
          iRetVal = FillBuffer(pRxBuffer,(unsigned char*)pBuffer, Size);

          if(iRetVal < 0)
          {
            LcdPrintf("Unable to fill buffer");
            LOOP_FOREVER();
          }
        }
        else
        { // we reach at the of file
        //close file
        closeFile();
        // reopen the file
        res = openFile();
        if(res != FR_OK) break;
        }
        if(g_uiPlayWaterMark == 0)
        {
          if(IsBufferSizeFilled(pRxBuffer,PLAY_WATERMARK) == TRUE)
          {
            g_uiPlayWaterMark = 1;
          }
        }
        g_iReceiveCount++;
      }
      osi_Sleep(1000);
    }
}

