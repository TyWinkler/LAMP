/*
 * LCD.h
 *
 *  Created on: Apr 10, 2017
 *      Author: Jon
 */

#ifndef LCD_H_
#define LCD_H_

#define DEFAULT_LCD_FONT g_sFontCm20;
#define LCD_Y_SHIFT        20
#define NEXT_LCD_CURSOR_Y  (g_lLcdCursorY + LCD_Y_SHIFT)
#define LCD_MESSAGE(msg)    {\
                           GrStringDrawCentered(&sContext, msg, -1,\
                           GrContextDpyWidthGet(&sContext) / 2, NEXT_LCD_CURSOR_Y, 0);\
                           g_lLcdCursorY += LCD_Y_SHIFT;\
                            }

void LCDReset(void);
void LCD(void);
void displaymytextnext(void);
void displaymytext(void);
int LcdPrintf(char *pcFormat, ...);
void clearScreen();
extern long g_lLcdCursorY;
extern long g_lLcdCursorX;
#endif /* LCD_H_ */
