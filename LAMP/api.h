/*
 * api.h
 *
 */
#include "ff.h"

#ifndef API_H_
#define API_H_

extern void apiOff();
extern void apiSetColorIm(unsigned long color);
extern void apiEditAlarm(int time, int themeId, int dow, int alarmId, int running);
extern void apiEditTheme(int themeId, long color, TCHAR* song);
extern void apiDeleteAlarm(int alarmId);
extern void apiDeleteTheme(int themeId);
extern void apiPlayTheme(int themeId);
extern void apiUpdateTime(unsigned int time);

typedef struct alarm{
    unsigned int alarmId;
    unsigned int themeId;
    unsigned int time;          //military time
    unsigned int dow;
    unsigned int running;
    unsigned int active;
} alarm;

typedef struct theme{
    unsigned int themeId;
    unsigned long color;
    TCHAR* song;
    unsigned char active;
} theme;

#endif /* API_H_ */
