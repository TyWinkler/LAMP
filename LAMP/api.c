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
#include <mxml.h>
#include "ff.h"

//Turns off speaker and lights
void apiOff(){

}

//immediately changes the color
void apiSetColorIm(unsigned long color){

}

//creates an alarm if one does not exist or edits an existing one
void apiEditAlarm(unsigned int time, unsigned char themeId, unsigned char dow, unsigned char alarmId, unsigned char running){

}

//creates a theme if one dose not exist or edits an existing one
void apiEditTheme(unsigned char themeId, unsigned long color, TCHAR* song){

}

//deletes an alarm if it exists
void apiDeleteAlarm(unsigned char alarmId){

}

//deletes a theme if it exists
void apiDeleteTheme(unsigned char themeId){

}

//plays theme immediately
void apiPlayTheme(unsigned char themeId){

}


//updates the time displayed on the device
void apiUpdateTime(unsigned int time){

}
