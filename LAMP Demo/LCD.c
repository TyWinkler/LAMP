
// Hardware & driverlib library includes

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "salowcc3200_ili9341.h"
#include "rom_map.h"
#include "rom.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "grlib.h"
#include "gpio.h"
#include "LPD8806.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

static tContext sContext;
extern const tDisplay g_ssalowCC3200_ili9341;

long g_lLcdCursorY = 30;
#define DEFAULT_LCD_FONT g_sFontCm20;
#define LCD_Y_SHIFT        20
#define NEXT_LCD_CURSOR_Y  (g_lLcdCursorY + LCD_Y_SHIFT)
#define LCD_MESSAGE(msg)    {\
                           GrStringDrawCentered(&sContext, msg, -1,\
                           GrContextDpyWidthGet(&sContext) / 2, NEXT_LCD_CURSOR_Y, 0);\
                           g_lLcdCursorY += LCD_Y_SHIFT;\
                            }

void LCDReset();
void displaymytextnext();
void displaymytext(void);
int LcdPrintf(char *pcFormat, ...);

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void LCDReset(){
    unsigned long temp;
        temp = 80000000 / (3 * 1000);
    //GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,0);
    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_4,0); //Pin 59
    UtilsDelay(10*temp);
    //GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,GPIO_PIN_0);
    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_4,GPIO_PIN_4);
}

void displaymytext(void){
    salowCC3200_ili9341Init();

    GrContextInit(&sContext, &g_ssalowCC3200_ili9341);
    GrContextBackgroundSet(&sContext, ClrWhite);
    GrContextForegroundSet(&sContext, ClrBlack);

    // Put the CC3200 Banner on screen.
    GrContextFontSet(&sContext, &g_sFontCmss28);

}

int LcdPrintf(char *pcFormat, ...){
    int iRet = 0;
    char *pcBuff, *pcTemp;
    int iSize = 256;
    va_list list;
    pcBuff = (char*)malloc(iSize);
    if(pcBuff == NULL){
      return -1;
    }
    while(1){
      va_start(list,pcFormat);
      iRet = vsnprintf(pcBuff,iSize,pcFormat,list);
      va_end(list);
      if(iRet > -1 && iRet < iSize){
          break;
      } else {
          iSize*=2;
          if((pcTemp=realloc(pcBuff,iSize))==NULL){
              iRet = -1;
              break;
          } else {
              pcBuff=pcTemp;
          }
      }
    }
    LCD_MESSAGE(pcBuff);
    free(pcBuff);
    return iRet;
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
void LCD( void *pvParameters ){
    LCDReset();
    displaymytext();
    unsigned long critical;
    while(1){
        critical = osi_EnterCritical();
        LCDReset();
        displaymytext();
        g_lLcdCursorY = 30;
        LcdPrintf(" ");
        LcdPrintf("The color is");
        LcdPrintf(" ");
        LcdPrintf("%#08x", myColor);
        osi_ExitCritical(critical);
        osi_Sleep(500);
    }
}

