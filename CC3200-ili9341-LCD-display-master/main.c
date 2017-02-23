//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - Display check
// Application Overview - This application showcases Timer's count capture 
//                        feature to measure frequency of an external signal.

//
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"
#include "timer.h"
#include "rom.h"
#include "rom_map.h"
#include "pin.h"
#include "grlib.h"
#include "spi.h"

// Common interface includes
#include "uart_if.h"
#include "pinmux.h"

#include "salowcc3200_ili9341.h"


#define APPLICATION_VERSION     "1.1.1"
#define APP_NAME        "Display check"
#define TIMER_FREQ      80000000
#define SPI_IF_BIT_RATE         100000

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************
static tContext sContext;
long g_lLcdCursorY = 30;
#define DEFAULT_LCD_FONT g_sFontCm20;
#define LCD_Y_SHIFT        20
#define NEXT_LCD_CURSOR_Y  (g_lLcdCursorY + LCD_Y_SHIFT)
#define LCD_MESSAGE(msg) 	{\
                           GrStringDrawCentered(&sContext, msg, -1,\
                           GrContextDpyWidthGet(&sContext) / 2, NEXT_LCD_CURSOR_Y, 0);\
                           g_lLcdCursorY += LCD_Y_SHIFT;\
							}




//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t\t  CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

void LCDReset();
void displaymytextnext();
void displaymytext(void);
int LcdPrintf(char *pcFormat, ...);

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}
void ConfigLcdScreen()
{

	//
	// Reset SPI
	//
	SPIReset(GSPI_BASE);

	//
	// Configure SPI interface
	//
	SPIConfigSetExpClk(GSPI_BASE,PRCMPeripheralClockGet(PRCM_GSPI),
					 SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
					 (
					 SPI_3PIN_MODE |
					 SPI_TURBO_OFF |

					 SPI_WL_8));


	//
	// Enable SPI for communication
	//
	SPIEnable(GSPI_BASE);
	//GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,GPIO_PIN_0);
	LCDReset();


	 displaymytext();
	 /*LCDReset();
	 displaymytextnext();*/


}
void LCDReset(){
	unsigned long temp;
		temp = 80000000 / (3 * 1000);
	GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,0);
	UtilsDelay(10*temp);
	GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,GPIO_PIN_0);

}
void displaymytextnext(){

	g_lLcdCursorY = 30;

	salowCC3200_ili9341Init();

		//
		// Initialize the graphics context.
		//
		//GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);


		 GrContextInit(&sContext, &g_ssalowCC3200_ili9341);

		 GrContextForegroundSet(&sContext, ClrBlack);

		 		//
		 		// Put the CC3200 Banner on screen.
		 		//
		 		GrContextFontSet(&sContext, &g_sFontCmss34i);
		 		GrStringDrawCentered(&sContext, "Salow", -1,
		 									GrContextDpyWidthGet(&sContext) / 2, 20, 0);

		 		LcdPrintf(" ");
			LcdPrintf("next text");
			LcdPrintf(" ");
			LcdPrintf("THank GOD");
}

void displaymytext(void){
	salowCC3200_ili9341Init();

		//
		// Initialize the graphics context.
		//
		//GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);


		 GrContextInit(&sContext, &g_ssalowCC3200_ili9341);

	GrContextForegroundSet(&sContext, ClrBlack);

		//
		// Put the CC3200 Banner on screen.
		//
		GrContextFontSet(&sContext, &g_sFontCmss28b);
		//GrStringDrawCentered(&sContext, "Salow", -1,
		//					GrContextDpyWidthGet(&sContext) / 2, 20, 0);


		//LcdPrintf(" the value is %d",aa);
		LcdPrintf(" ");
		//GrContextForegroundSet(&sContext, ClrMaroon);
		//GrContextFontSet(&sContext, &g_sFontCmsc28);
		LcdPrintf("Our LCD is Working");
		LcdPrintf(" ");
		LcdPrintf("Ty Winkler");

		//GrContextForegroundSet(&sContext,ClrMintCream);
		//LcdPrintf("www.salow.co");
}


int LcdPrintf(char *pcFormat, ...)
{
	int iRet = 0;
	char *pcBuff, *pcTemp;
	int iSize = 256;

	va_list list;
	pcBuff = (char*)malloc(iSize);
	if(pcBuff == NULL)
	{
	  return -1;
	}
	while(1)
	{
	  va_start(list,pcFormat);
	  iRet = vsnprintf(pcBuff,iSize,pcFormat,list);
	  va_end(list);
	  if(iRet > -1 && iRet < iSize)
	  {
		  break;
	  }
	  else
	  {
		  iSize*=2;
		  if((pcTemp=realloc(pcBuff,iSize))==NULL)
		  {
			  iRet = -1;
			  break;
		  }
		  else
		  {
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
//! Main  Function
//
//*****************************************************************************
int main()
{

    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Pinmux for UART
    //
    PinMuxConfig();
    ConfigLcdScreen();

    //
    // Configuring UART
    //
    InitTerm();

    //
    // Display Application Banner
    //
    DisplayBanner(APP_NAME);

    //
    // Enable pull down
    //

    //LcdPrintf("Connected to AP SALOW");





}
