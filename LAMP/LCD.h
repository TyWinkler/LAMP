/*
 * LCD.h
 *
 *  Created on: Apr 10, 2017
 *      Author: Jon
 */

#ifndef LCD_H_
#define LCD_H_

void LCDReset(void);
void displaymytextnext(void);
void displaymytext(void);
int LcdPrintf(char *pcFormat, ...);
void clearScreen();


#endif /* LCD_H_ */
