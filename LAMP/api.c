//*****************************************************************************
//
// Application Name     - API Calls
// Application Overview - Takes the the parsed inputs from network calls and impliments the proper functions
// Application Details  - https://github.com/TyWinkler/LAMP/tree/master/LAMP
// Authors              - Ty Winkler
//
//*****************************************************************************

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ff.h"
#include "api.h"
#include "LCD.h"
#include "osi.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "prcm.h"
#include "LPD8806.h"
#include "circ_buff.h"
#define BUFFSIZE                1024*10

//struct theme{
//    unsigned long color;
//    TCHAR* song;
//    unsigned char active;
//}
theme themes[30];

//struct alarm{
//    unsigned char themeId;
//    unsigned int time;          //military time
//    unsigned char dow;
//    unsigned char running;
//    unsigned char active;
//}
alarm alarms[30];

extern unsigned long myColor;
extern unsigned char g_ucSpkrStartFlag;
//extern const unsigned char* myWav;
extern unsigned int currentTime;
extern unsigned char songChanged;
extern char songname[];
extern unsigned int specialColor;
extern int colorStage;
extern tCircularBuffer *pRxBuffer;

FIL tafp;
FATFS tafs;
FRESULT tares;
DIR tadir;
UINT taSize;

extern OsiSyncObj_t g_ControllerSyncObj;
extern OsiSyncObj_t g_SpeakerSyncObj;

//Turns off speaker and lights
void apiOff(){
    myColor = 0x000000;
    allColor(colorHex(myColor));
    g_ucSpkrStartFlag = 0;
    specialColor = 0;
    FillZeroes(pRxBuffer, BUFFSIZE);
}

//immediately changes the color
void apiSetColorIm(unsigned long color){
    //unsigned long key = osi_EnterCritical();
    myColor = color;
    specialColor = 0;
    //osi_ExitCritical(key);
    allColor(colorHex(myColor));
}

//creates an alarm if one does not exist or edits an existing one
void apiEditAlarm(int time, int themeId, int dow, int alarmId, int running){
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(alarms[i].alarmId == alarmId && alarms[i].active){
            storageId = i;
            break;
        }
    }
    if(storageId == -1){
        for(i = 0; i < 30; i++){
            if(alarms[i].active == 0){
                alarms[i].alarmId = alarmId;
                storageId = i;
            }
        }
    }
    if(storageId != -1){
        if(time != -1){
            alarms[storageId].time = time;
        }
        if(themeId != -1){
            alarms[storageId].themeId = themeId;
        }
        if(dow != -1){
            alarms[storageId].dow = dow;
        }
        if(running != -1){
            alarms[storageId].running = running;
        }
        if(alarms[storageId].active == 0){
            alarms[storageId].active = 1;
        }
        storeAlarms();
    }
}

//creates a theme if one dose not exist or edits an existing one
void apiEditTheme(int themeId, long color, char* song, int special){
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(themes[i].themeId == themeId){
#ifdef DEBUG2
            LcdPrintf("Found theme at %d",i);
#endif
            storageId = i;
            break;
        }
    }
    if(storageId == -1){
#ifdef DEBUG2
        LcdPrintf("Looking for spot to store");
#endif
        for(i = 0; i < 30; i++){
            if(themes[i].active == 0){
#ifdef DEBUG2
                LcdPrintf("Stored at %d",i);
#endif
                themes[i].themeId = themeId;
                storageId = i;
                break;
            }
        }
    }
    if(storageId != -1){
#ifdef DEBUG
        clearScreen();
        LcdPrintf(song);
#endif
        if(color != NULL){
            themes[storageId].color = color;
        }
        strcpy(themes[storageId].song, song);
#ifdef DEBUG
        LcdPrintf(themes[storageId].song);
#endif
        if(themes[themeId].active == 0){
            themes[storageId].active = 1;
        }
        themes[storageId].special = special;
        storeThemes();
    }
}

//deletes an alarm if it exists
void apiDeleteAlarm(int alarmId){
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(alarms[i].alarmId == alarmId){
            storageId = i;
            break;
        }
    }
    if(storageId != -1){
        alarms[storageId].active = 0;
        storeAlarms();
    }
}

//deletes a theme if it exists
void apiDeleteTheme(int themeId){
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(themes[i].themeId == themeId){
            storageId = i;
            break;
        }
    }
    if(storageId != -1){
        themes[storageId].active = 0;
        storeThemes();
    }
}

//plays theme immediately
void apiPlayTheme(int themeId){
    //unsigned long key = osi_EnterCritical();
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(themes[i].themeId == themeId){
#ifdef DEBUG
            //clearScreen();
            LcdPrintf("Found at %d",i);
#endif
            storageId = i;
            break;
        }
    }
    if(storageId != -1){
        myColor = themes[storageId].color;
        allColor(colorHex(myColor));
#ifdef DEBUG
                LcdPrintf("My color is %#08x",myColor);
#endif
        if(themes[storageId].song != NULL){
            strcpy(songname,themes[storageId].song);
            songChanged = 1;
            if(strcmp(songname,"NA") == 0){
                g_ucSpkrStartFlag = 0;
                //FillZeroes(pRxBuffer, BUFFSIZE);
            } else {
                g_ucSpkrStartFlag = 1;
            }
        }
        specialColor = themes[storageId].special;
        colorStage = 0;
    }
    //osi_ExitCritical(key);
    osi_SyncObjSignal(&g_SpeakerSyncObj);
    osi_SyncObjSignal(&g_ControllerSyncObj);
}

//updates the time displayed on the device
void apiUpdateTime(unsigned long time){
    unsigned long key = osi_EnterCritical();
    currentTime = time;
    //PRCMRTCSet(currentTime,0);
    osi_ExitCritical(key);
    osi_SyncObjSignal(&g_ControllerSyncObj);
}

void openTADir(){
    tares = FR_NOT_READY;
    while(tares != FR_OK){
        //LcdPrintf("Trying to open");
        f_mount(&tafs,"0",1);
        tares = f_opendir(&tadir,"/");
    }
}

void closeTADir(){
    tares = FR_NOT_READY;
    while(tares != FR_OK){
        //LcdPrintf("Trying to open");
        f_closedir(&tadir);
        tares = f_mount(&tafs,"0",1);
    }
}

void getAlarms(){
    unsigned char temp = g_ucSpkrStartFlag;
    g_ucSpkrStartFlag = 0;
    unsigned long key = osi_EnterCritical();
    openTADir();
    tares = f_open(&tafp,"alarms",FA_READ);
    if(tares == FR_OK) {
        f_read(&tafp,alarms,sizeof(struct alarm) * 30,&taSize);
        f_close(&tafp);
    } else {
        LcdPrintf("Failed to open alarms");
    }
    closeTADir();
    osi_ExitCritical(key);
    if(temp){
        g_ucSpkrStartFlag = 1;
    }
}

void storeAlarms(){
    unsigned char temp = g_ucSpkrStartFlag;
    g_ucSpkrStartFlag = 0;
    unsigned long key = osi_EnterCritical();
    openTADir();
    tares = f_open(&tafp,"alarms",FA_CREATE_ALWAYS|FA_WRITE);
    if(tares == FR_OK) {
        f_write(&tafp,alarms,sizeof(struct alarm) * 30,&taSize);
        f_close(&tafp);
    } else {
        LcdPrintf("Failed to create a new file");
    }
    closeTADir();
    osi_ExitCritical(key);
    if(temp){
        g_ucSpkrStartFlag = 1;
    }
}

void getThemes(){
    unsigned char temp = g_ucSpkrStartFlag;
    g_ucSpkrStartFlag = 0;
    unsigned long key = osi_EnterCritical();
    openTADir();
    tares = f_open(&tafp,"themes",FA_READ);
    if(tares == FR_OK) {
        f_read(&tafp,themes,sizeof(struct theme) * 30,&taSize);
        f_close(&tafp);
    } else {
        LcdPrintf("Failed to open themes");
    }
    closeTADir();
    osi_ExitCritical(key);
    if(temp){
        g_ucSpkrStartFlag = 1;
    }
}

void storeThemes(){
    unsigned char temp = g_ucSpkrStartFlag;
    g_ucSpkrStartFlag = 0;
    unsigned long key = osi_EnterCritical();
    openTADir();
    tares = f_open(&tafp,"themes",FA_CREATE_ALWAYS|FA_WRITE);
    if(tares == FR_OK) {
        f_write(&tafp,themes,sizeof(struct theme) * 30,&taSize);
        f_close(&tafp);
    } else {
        LcdPrintf("Failed to create a new file");
    }
    closeTADir();
    osi_ExitCritical(key);
    if(temp){
        g_ucSpkrStartFlag = 1;
    }
}
