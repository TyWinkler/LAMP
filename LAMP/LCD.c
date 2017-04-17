
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
#include "osi.h"
#include "ff.h"
#include "LCD.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

tContext sContext;
extern const tDisplay g_ssalowCC3200_ili9341;
long g_lLcdCursorY = 30;

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void LCDReset(){
    unsigned long temp;
    temp = 80000000 / (3 * 1000);
    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_4,0); //Pin 59
    UtilsDelay(10*temp);
    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_4,GPIO_PIN_4);
}

void displaymytext(void){
    salowCC3200_ili9341Init();
    GrContextInit(&sContext, &g_ssalowCC3200_ili9341);
    GrContextBackgroundSet(&sContext, ClrBlack);
    GrContextForegroundSet(&sContext, ClrBlack);
    const int width = GrContextDpyWidthGet(&sContext);
    const int height = GrContextDpyHeightGet(&sContext);
    tRectangle screen = {0,0,width,height};
    DpyRectFill(&g_ssalowCC3200_ili9341, &screen, ClrBlack);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, &g_sFontCmss28);
}

void clearScreen(){
    GrContextBackgroundSet(&sContext, ClrBlack);
    GrContextForegroundSet(&sContext, ClrBlack);
    const int width = GrContextDpyWidthGet(&sContext);
    const int height = GrContextDpyHeightGet(&sContext);
    tRectangle screen = {0,0,width,height};
    DpyRectFill(&g_ssalowCC3200_ili9341, &screen, ClrBlack);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, &g_sFontCmss28);
    g_lLcdCursorY = 30;
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
