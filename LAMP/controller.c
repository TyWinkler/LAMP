// Hardware & driverlib library includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include "salowcc3200_ili9341.h"
#include "api.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "osi.h"
#include "prcm.h"

#define LEDoff 0x00000000

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern unsigned char g_ucSpkrStartFlag;
extern unsigned long currentColor;
extern alarm alarms[30];
extern theme themes[30];
extern TCHAR* myWav;
time_t currentTime;
struct tm* ts;
char timeBuf[20];
static alarm* currentAlarm;
static theme* currentTheme;
static char currentAlarmHasPlayed = 0;
static char snoozing = 0;
static int snoozetime = 0;

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void playTheme(theme* playme){
    myWav = playme->song;
    g_ucSpkrStartFlag = true;
    currentColor = playme->color;
}

void snooze(void){
    g_ucSpkrStartFlag = false;
    currentColor = LEDoff;
    snoozing = 1;
    snoozetime = (currentAlarm->time%100 + 10)%60;
}

void alarmOff(void){
    g_ucSpkrStartFlag = false;
    currentColor = LEDoff;
    snoozing = 0;
    currentAlarm->running = 0;
}

void hasAlarmPlayed(void){
    if(currentAlarm->time/100 == ts->tm_hour &&
       currentAlarm->time%100 == ts->tm_min &&
       currentAlarm->running == 1){
        currentAlarmHasPlayed = 1;
    } else currentAlarmHasPlayed = 0;
}

void Controller( void *pvParameters ){
    //PRCMRTCInUseSet();
    //PRCMRTCSet(currentTime,0);
    //unsigned short throwaway;
    //_tz.timezone = 0;
    //time_t* toss = 0;
   // int years = currentTime / 30758400;
    //int  = currentTime / 30758400;
    static int prev_min = 0;

    while(1){
        //PRCMRTCGet((unsigned long*)&currentTime, &throwaway);
        time(&currentTime);
        ts = localtime(&currentTime);
        if(prev_min != ts->tm_min){
            strftime(timeBuf, 80, "%b %d %I:%M %p", ts);
//          LcdPrintf(timeBuf);         // update LCD time
            int i;
            for(i = 0; i < 30; i++){
                if(alarms[i].active &&
                   alarms[i].time/100 == ts->tm_hour &&
                   alarms[i].time%100 == ts->tm_min &&
                   alarms[i].running == 0 &&
                   !currentAlarmHasPlayed &&
                   !snoozing)
                {
                    currentAlarm = &alarms[i];
                    currentTheme = &themes[alarms[i].themeId];
                    playTheme(currentTheme);
                    alarms[i].running = 1;
                    break;
                }
            }
            if(snoozing && snoozetime == ts->tm_min){
                snoozing = 0;
                playTheme(currentTheme);
            }
            hasAlarmPlayed();
            prev_min = ts->tm_min;
        }
        osi_Sleep(500);
    }
}
