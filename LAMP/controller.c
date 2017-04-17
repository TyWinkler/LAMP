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
#include "LCD.h"
#include "LPD8806.h"
#include "grlib.h"

#define LEDoff 0x00000000
//#define DEBUG2

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern unsigned char g_ucSpkrStartFlag;
extern unsigned long myColor;
extern unsigned char songChanged;
extern alarm alarms[30];
extern theme themes[30];
//extern TCHAR* myWav;
time_t currentTime;
struct tm* ts;
char myTime[20];
char prevTime[20];
static alarm* currentAlarm;
static theme* currentTheme;
static char currentAlarmHasPlayed = 0;
static char snoozing = 0;
static int snoozetime = 0;
static int timeHasChanged = 0;
extern const tDisplay g_ssalowCC3200_ili9341;
extern tContext sContext;

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void playTheme(theme* playme){
    //myWav = playme->song;
    g_ucSpkrStartFlag = true;
    songChanged = true;
    myColor = playme->color;
}

void snooze(void){
    g_ucSpkrStartFlag = false;
    myColor = LEDoff;
    snoozing = 1;
    snoozetime = (currentAlarm->time%100 + 10)%60;
}

void alarmOff(void){
    g_ucSpkrStartFlag = false;
    myColor = LEDoff;
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

    PRCMRTCInUseSet();
    time(&currentTime);
    PRCMRTCSet(currentTime,0);
    unsigned short throwaway;
    //_tz.timezone = 0;
    //time_t* toss = 0;
   // int years = currentTime / 30758400;
    //int  = currentTime / 30758400;
    static int prev_min = 0;
    reset();
    allColor(colorHex(LEDoff));

#ifdef DEBUG2
    apiEditTheme(1,0x001010,"stuck.wav");
    clearScreen();
    g_lLcdCursorY = 120;
    LcdPrintf("Active = %d",themes[0].active);
    apiPlayTheme(1);
#endif

    while(1){

        PRCMRTCGet((unsigned long*)&currentTime, &throwaway);
        ts = localtime(&currentTime);

        if(prev_min != ts->tm_min){
            strftime(myTime, 80, "%b %d %I:%M %p", ts);
            timeHasChanged = 1;
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
#ifndef DEBUG
        LCD();
#endif
        LED();

        osi_Sleep(500);
    }
}

//*****************************************************************************
//
//! LCD Routine
//!
//! \param pvParameters     Parameters to the task's entry function
//!
//! \return None
//
//*****************************************************************************
void LCD(void){
    static unsigned long prevColor = 1;
    long dispColor;

            if(timeHasChanged){
                g_lLcdCursorY = 35;
                GrContextForegroundSet(&sContext, ClrBlack);
                LcdPrintf(prevTime);
                GrContextForegroundSet(&sContext, ClrWhite);
                g_lLcdCursorY = 35;
                LcdPrintf(myTime);
                timeHasChanged = 0;
            }
            if(prevColor != myColor){
                g_lLcdCursorY = 70;
                GrContextForegroundSet(&sContext, ClrBlack);
                dispColor = prevColor & 0x00FFFFFF;
                LcdPrintf("");
                LcdPrintf("%#08x", dispColor);

                g_lLcdCursorY = 70;
                GrContextForegroundSet(&sContext, ClrWhite);
                LcdPrintf("The current color is%n");
                dispColor = myColor & 0x00FFFFFF;
                LcdPrintf("%#08x", dispColor);
            }

            strcpy(prevTime,myTime);
            prevColor = myColor;
//            LEDWrite = 1;
}

//*****************************************************************************
//
//! LED Routine
//!
//! \param pvParameters     Parameters to the task's entry function
//!
//! \return None
//
//*****************************************************************************
void LED(void){
    static int prev_color = 0;
    if(prev_color != myColor){
        reset();
        allColor(colorHex(myColor));
        prev_color = myColor;
    }
}
