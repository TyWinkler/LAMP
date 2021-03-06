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
#define BUFFSIZE                1024*10
#define PLAY_WATERMARK          50*1024
unsigned char pBuffer[BUFFSIZE];
unsigned char g_ucSpkrStartFlag;
unsigned char songChanged = 0;
unsigned int seekVal = 0;

FIL songfp;
FATFS songfs;
FRESULT songres;
DIR songdir;
UINT songSize;

char songname[30] = "stuck.wav";
const TCHAR* myWav = songname;

extern unsigned long  g_ulStatus;
extern unsigned char g_uiPlayWaterMark;
extern unsigned char g_loopback;
//unsigned char speaker_data[16*1024];
extern tCircularBuffer *pRxBuffer;

extern OsiSyncObj_t g_SpeakerSyncObj;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************
void openSongDir(){
    songres = FR_NOT_READY;
    while(songres != FR_OK){
        //LcdPrintf("Trying to open");
        f_mount(&songfs,"0",1);
        songres = f_opendir(&songdir,"/");
    }
}

void closeSongDir(){
    songres = FR_NOT_READY;
    while(songres != FR_OK){
        //LcdPrintf("Trying to open");
        f_closedir(&songdir);
        songres = f_mount(&songfs,"0",1);
    }
}

void openFile(){
    if(songChanged){
        seekVal = 0;
        songChanged = 0;
        FillZeroes(pRxBuffer, BUFFSIZE);
    }
    Message("\n\rReading user file...\n\r");
    songres = f_open(&songfp,myWav,FA_READ);
    f_lseek(&songfp,44 + seekVal);
}

void closeFile(){
    f_close(&songfp);
}

void readFile(){
    openSongDir();
    openFile();
    if(songres == FR_OK)
    {
        f_read(&songfp,pBuffer,BUFFSIZE,&songSize);
        seekVal += songSize;
        Report("Read : %d Bytes\n\n\r",songSize);
        Report("%s",pBuffer);
    }
    else
    {
        Report("Failed to open %s\n\r",myWav);
    }
    closeFile();
    closeSongDir();
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
void Speaker( void *pvParameters ){
    long iRetVal;
    //f_mount(&fs,"0",1);
    //res = f_opendir(&dir,"/");
    //open file
    openFile();
    g_ucSpkrStartFlag = 0;
    while(1){
        osi_SyncObjWait(&g_SpeakerSyncObj,20);
        while(g_ucSpkrStartFlag){
            osi_SyncObjWait(&g_SpeakerSyncObj,20);
            // Read from file and discard wav header
            //unsigned long key = osi_EnterCritical();
            readFile();
            /* Wait to avoid buffer overflow as reading speed is faster than playback */
            while((IsBufferSizeFilled(pRxBuffer,PLAY_WATERMARK) == TRUE)){}
            if( songSize > 0){
                iRetVal = FillBuffer(pRxBuffer,(unsigned char*)pBuffer, songSize);
                if(iRetVal < 0){
                    LcdPrintf("Unable to fill buffer");
                    LOOP_FOREVER();
                }
            } else { // we reach at the of file
                songChanged = 1;
                //close file
                closeFile();
                // reopen the file
                openFile();
            }
            g_iReceiveCount++;
            //osi_ExitCritical(key);
        }
        //FillZeroes(pRxBuffer, BUFFSIZE);
    }
}
