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
#include "rom_map.h"
#include "utils.h"

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
int wifiConnected = 0;
int wifiChanged = 0;
int colorStage = 0;
extern const tDisplay g_ssalowCC3200_ili9341;
extern tContext sContext;
unsigned int specialColor = 0;
unsigned long key;

extern OsiSyncObj_t g_ControllerSyncObj;
extern OsiSyncObj_t g_SpeakerSyncObj;

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

int once = 0; // test

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
    allColor(colorHex(LEDoff));
//    getThemes();
//    getAlarms();
    PRCMRTCGet((unsigned long*)&currentTime, &throwaway);
    ts = localtime(&currentTime);
    strftime(myTime, 80, "%b %d %I:%M %p", ts);
    timeHasChanged = 1;
    LCD();

#define RESETSPECIAL
#ifdef RESETSPECIAL
    apiEditTheme(0,0x000000,"NA",1);
    apiEditTheme(1,0x000000,"NA",2);
    apiEditTheme(2,0x000000,"NA",3);

#endif

    while(1){

        osi_SyncObjWait(&g_ControllerSyncObj,1000);
        //PRCMRTCGet((unsigned long*)&currentTime, &throwaway);
        //ts = localtime(&currentTime);
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
        //osi_ExitCritical(key);
        //osi_Sleep(100);
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
            if(wifiConnected){
                if(wifiChanged){
                    g_lLcdCursorY = 0;
                    g_lLcdCursorX = 30;
                    GrContextForegroundSet(&sContext, ClrBlack);
                    GrContextFontSet(&sContext, &g_sFontCmss16);
                    LcdPrintf("Please connect to WiFi!");
                    GrContextFontSet(&sContext, &g_sFontCmss28);
                    wifiChanged = 0;
                }
                g_lLcdCursorY = 0;
                g_lLcdCursorX = 30;
                GrContextForegroundSet(&sContext, ClrLime);
                GrContextFontSet(&sContext, &g_sFontCmss16);
                LcdPrintf("Connected!");
                GrContextFontSet(&sContext, &g_sFontCmss28);
            } else {
                if(wifiChanged){
                    g_lLcdCursorY = 0;
                    g_lLcdCursorX = 30;
                    GrContextForegroundSet(&sContext, ClrBlack);
                    GrContextFontSet(&sContext, &g_sFontCmss16);
                    LcdPrintf("Connected!");
                    GrContextFontSet(&sContext, &g_sFontCmss28);
                    wifiChanged = 0;
                }
                g_lLcdCursorY = 0;
                g_lLcdCursorX = 30;
                GrContextForegroundSet(&sContext, ClrBlue);
                GrContextFontSet(&sContext, &g_sFontCmss16);
                LcdPrintf("Please connect to WiFi!");
                GrContextFontSet(&sContext, &g_sFontCmss28);
            }
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

long random_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = rand();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return (x/bin_size) + 1;
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
//    static int prev_color = 0;
//    if(prev_color != myColor && specialColor == 0){
//        allColor(colorHex(myColor));
//        prev_color = myColor;
//    } else
    if(specialColor == 1){
        //Sunrise
        if(colorStage == 0){
            myColor = 0x02004f;
            allColor(colorHex(myColor));
            if(myColor == 0x2004f){
                colorStage++;
            }
        } else if(colorStage == 1){
            myColor = mergeColors(myColor,0x2824c1,1);
            allColor(colorHex(myColor));
            if(myColor == 0x2824c1){
                colorStage++;
            }
        } else if(colorStage == 2){
            myColor = mergeColors(myColor,0x899317,1);
            allColor(colorHex(myColor));
            if(myColor == 0x899317){
                colorStage++;
            }
        } else if(colorStage == 3){
            myColor = mergeColors(myColor,0xfff054,1);
            allColor(colorHex(myColor));
            if(myColor == 0xfff054){
                colorStage++;
            }
        } else if(colorStage == 4){
            myColor = mergeColors(myColor,0xfefffc,1);
            allColor(colorHex(myColor));
            if(myColor == 0xfefffc){
                colorStage++;
            }
        }

    } else if(specialColor == 2){
        //Fire
        if(colorStage == 0){
            myColor = 0xFF0000;
            allColor(colorHex(myColor));
            if(myColor == 0xFF0000){
                colorStage++;
            }
        } else if(colorStage == 1){
            myColor = mergeColors(myColor,0xE25822,1);
            allColor(colorHex(myColor));
            if(myColor == 0xE25822){
                colorStage = random_at_most(3);
            }
        } else if(colorStage == 2){
            myColor = mergeColors(myColor,0x6F1F00,1);
            allColor(colorHex(myColor));
            if(myColor == 0x6F1F00){
                colorStage = random_at_most(3);
            }
        } else if(colorStage == 3){
            myColor = mergeColors(myColor,0xBC3500,1);
            allColor(colorHex(myColor));
            if(myColor == 0xBC3500){
                colorStage = random_at_most(3);
            }
        } else if(colorStage == 4){
            myColor = mergeColors(myColor,0xFFB699,1);
            allColor(colorHex(myColor));
            if(myColor == 0xFFB699){
                colorStage = random_at_most(3);
            }
        }
    } else if(specialColor == 3){
        //Rainbow
        if(colorStage == 0){
            myColor = 0x9400D3;
            allColor(colorHex(myColor));
            if(myColor == 0x9400D3){
                colorStage++;
            }
        } else if(colorStage == 1){
            myColor = mergeColors(myColor,0x4B0082,1);
            allColor(colorHex(myColor));
            if(myColor == 0x4B0082){
                colorStage++;
            }
        } else if(colorStage == 2){
            myColor = mergeColors(myColor,0x0000FF,1);
            allColor(colorHex(myColor));
            if(myColor == 0x0000FF){
                colorStage++;
            }
        } else if(colorStage == 3){
            myColor = mergeColors(myColor,0x00FF00,1);
            allColor(colorHex(myColor));
            if(myColor == 0x00FF00){
                colorStage++;
            }
        } else if(colorStage == 4){
            myColor = mergeColors(myColor,0xFFFF00,1);
            allColor(colorHex(myColor));
            if(myColor == 0xFFFF00){
                colorStage++;
            }
        } else if(colorStage == 5){
            myColor = mergeColors(myColor,0xFF7F00,1);
            allColor(colorHex(myColor));
            if(myColor == 0xFF7F00){
                colorStage++;
            }
        } else if(colorStage == 6){
            myColor = mergeColors(myColor,0xFF0000,1);
            allColor(colorHex(myColor));
            if(myColor == 0xFF0000){
                colorStage++;
            }
        } else if(colorStage == 7){
            myColor = mergeColors(myColor,0x9400D3,1);
            allColor(colorHex(myColor));
            if(myColor == 0x9400D3){
                colorStage = 1;
            }
        }
    }
}
