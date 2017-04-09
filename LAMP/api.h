/*
 * api.h
 *
 */
#include "ff.h"

#ifndef API_H_
#define API_H_

extern void apiOff();
extern void apiSetColorIm(unsigned long color);
extern void apiEditAlarm(unsigned int time, unsigned char themeId, unsigned char dow, unsigned char alarmId, unsigned char running);
extern void apiEditTheme(unsigned char themeId, unsigned long color, TCHAR* song);
extern void apiDeleteAlarm(unsigned char alarmId);
extern void apiDeleteTheme(unsigned char themeId);
extern void apiPlayTheme(unsigned char themeId);
extern void apiUpdateTime(unsigned int time);

typedef struct alarm{
    unsigned char themeId;
    unsigned int time;          //military time
    unsigned char dow;
    unsigned char running;
    unsigned char active;
} alarm;

typedef struct theme{
    unsigned long color;
    TCHAR* song;
    unsigned char active;
} theme;

#endif /* API_H_ */
