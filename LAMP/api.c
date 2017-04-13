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
extern const TCHAR* myWav;
extern unsigned int currentTime;
extern unsigned char songChanged;

//Turns off speaker and lights
void apiOff(){
    myColor = 0x000000;
    g_ucSpkrStartFlag = 0;
}

//immediately changes the color
void apiSetColorIm(unsigned long color){
    unsigned long key = osi_EnterCritical();
    myColor = color;
    osi_ExitCritical(key);
}

//creates an alarm if one does not exist or edits an existing one
void apiEditAlarm(int time, int themeId, int dow, int alarmId, int running){
    int i;
    int storageId = -1;
    for(i = 0; i < 30; i++){
        if(alarms[i].alarmId == alarmId){
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
    }
}

//creates a theme if one dose not exist or edits an existing one
void apiEditTheme(int themeId, long color, TCHAR* song){
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
#ifdef DEBUG2
        LcdPrintf("Setting variables");
#endif
        if(color != NULL){
            themes[storageId].color = color;
        }
        if(!strcmp(song,"NA")){
            themes[storageId].song = song;
        }
        if(themes[themeId].active == 0){
            themes[storageId].active = 1;
        }
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
    }
}

//plays theme immediately
void apiPlayTheme(int themeId){
    unsigned long key = osi_EnterCritical();
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
#ifdef DEBUG
                LcdPrintf("My color is %#08x",myColor);
#endif
        if(themes[storageId].song != NULL){
            myWav = themes[storageId].song;
#ifdef DEBUG
            LcdPrintf(myWav);
#endif
            songChanged = 1;
            if(myWav == "NA"){
                g_ucSpkrStartFlag = 0;
            } else {
                g_ucSpkrStartFlag = 1;
            }
        }
    }
    osi_ExitCritical(key);
}


//updates the time displayed on the device
void apiUpdateTime(unsigned long time){
    unsigned long key = osi_EnterCritical();
    currentTime = time;
    PRCMRTCSet(currentTime,0);
    osi_ExitCritical(key);
}
