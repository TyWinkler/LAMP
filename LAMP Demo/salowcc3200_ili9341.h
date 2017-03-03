/*
 * salowCC3200_ili9341.h
 *
 *  Created on: Apr 10, 2016
 *      Author: embedded
 */
#ifndef SALOWCC3200_ILI9341_H_
#define SALOWCC3200_ILI9341_H_

extern void salowCC3200_ili9341Init(void);
extern void salowCC3200_ili9341BacklightOn(void);
extern unsigned short salowCC3200_ili9341ControllerIdGet(void);
extern void salowCC3200_ili9341BacklightOff(void);
extern const tDisplay g_ssalowCC3200_ili9341;

#endif /* SALOWCC3200_ILI9341_H_ */
