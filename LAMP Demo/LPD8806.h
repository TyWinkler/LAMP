//*****************************************************************************
// pinmux.h
//
//*****************************************************************************

#ifndef LPD8806_H_
#define LPD8806_H_

extern void LPD8806Init(void);
extern void allColor(unsigned long color);
extern void reset(void);
extern void display(void);
extern unsigned long color(unsigned char r, unsigned char g, unsigned char b);
extern unsigned long colorHex(unsigned long hex);
extern void setPixel(unsigned int p, unsigned long color);
extern void clearStrip(void);

#endif /* LPD8806_H_ */
