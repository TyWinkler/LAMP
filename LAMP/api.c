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

struct theme{
    unsigned long color;
    TCHAR* song;
    unsigned char active = 0;
} themes[30];

struct alarm{
    unsigned char themeId;
    unsigned int time;
    unsigned char dow;
    unsigned char running;
    unsigned char active = 0;
} alarms[30];

extern unsigned long myColor;
extern unsigned char g_ucSpkrStartFlag;
extern const TCHAR* myWav;
extern unsigned int currentTime;

//Turns off speaker and lights
void apiOff(){
    myColor = 0x000000;
    g_ucSpkrStartFlag = 0;
}

//immediately changes the color
void apiSetColorIm(unsigned long color){
    myColor = color;
}

//creates an alarm if one does not exist or edits an existing one
void apiEditAlarm(unsigned int time, unsigned char themeId, unsigned char dow, unsigned char alarmId, unsigned char running){
    if(time != NULL){
        alarms[alarmId].time = time;
    }
    if(themeId != NULL){
        alarms[alarmId].themeId = themeId;
    }
    if(dow != NULL){
        alarms[alarmId].dow = dow;
    }
    if(running != NULL){
        alarms[alarmId].running = running;
    }
    if(alarms[alarmId].active == 0){
        alarms[alarmId].active = 1;
    }
}

//creates a theme if one dose not exist or edits an existing one
void apiEditTheme(unsigned char themeId, unsigned long color, TCHAR* song){
    if(color != NULL){
        themes[themeId].color = color;
    }
    if(!strcmp(song,"NA")){
        themes[themeId].song = song;
    }
    if(themes[themeId].active == 0){
        themes[themeId].active = 1;
    }
}

//deletes an alarm if it exists
void apiDeleteAlarm(unsigned char alarmId){
    alarms[alarmId].active = 0;
}

//deletes a theme if it exists
void apiDeleteTheme(unsigned char themeId){
    themes[themeId].active = 0;
}

//plays theme immediately
void apiPlayTheme(unsigned char themeId){
    myColor = themes[themeId].color;
    if(themes[themeId].song != NULL){
        myWav = themes[themeId].song;
        g_ucSpkrStartFlag = 1;
    }
}


//updates the time displayed on the device
void apiUpdateTime(unsigned int time){
    currentTime = time;
}
