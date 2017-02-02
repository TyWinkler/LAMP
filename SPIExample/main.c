//*****************************************************************************
//
// Application Name     - SPI Example for interfacing LPD8806 with CC3200 using
//                        Code Composer Studio (CCS)
// Authors              - Ty Winkler & Jonathan Ambrose
//
//*****************************************************************************

// Standard includes
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"

// Common interface includes
#include "uart_if.h"
#include "pinmux.h"

// LPD8806 interface includes
#include "LPD8806.h"

#define APPLICATION_VERSION     "1.1.1"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

// Colors go in GRB order for the LPD8806
unsigned long clear = 0x000000;
unsigned long red = 0x00FF00;
unsigned long green = 0xFF0000;
unsigned long blue = 0x0000FF;
unsigned long white = 0xFFFFFF;

// Definitions used by the board initialization
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! Board Initialization & Configuration
//! This is TI's Initilization for the board
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void){

// In case of TI-RTOS vector table is initialize by OS itself
#ifndef USE_TIRTOS

  // Set vector table base
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main(){
    // Initialize Board configurations
    BoardInit();

    // Muxing UART and SPI lines.
    PinMuxConfig();

    // Initialize the SPI Interface
    LPD8806Init();

    setPixel(0,colorHex(0xFF0000));
    setPixel(1,color(255,0,255));
    setPixel(2,red);
    setPixel(3,blue);
    display();

    while(1)
    {
    }

}

