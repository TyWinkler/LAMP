//*****************************************************************************
// pinmux.c
//
// configure the device pins for different peripheral signals
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

// This file was automatically generated on 7/21/2014 at 3:06:20 PM
// by TI PinMux version 3.0.334
//
//*****************************************************************************

#include "pinmux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "prcm.h"
#include <sdhost.h>

//*****************************************************************************
void
PinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks 
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);

    //
    // Configure PIN_06 for SDHost0 SDCARD_DATA
    //
    MAP_PinTypeSDHost(PIN_06, PIN_MODE_8);
    MAP_PinConfigSet(PIN_06,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);
    //MAP_PinTypeSDHost(PIN_64, PIN_MODE_6);
    //MAP_PinConfigSet(PIN_64,PIN_STRENGTH_6MA, PIN_TYPE_STD_PU);

    //
    // Configure PIN_01 for SDHost0 SDCARD_CLK
    //
    MAP_PinTypeSDHost(PIN_07, PIN_MODE_8);
    MAP_PinDirModeSet(PIN_07,PIN_DIR_MODE_OUT);
    //MAP_PinTypeSDHost(PIN_01, PIN_MODE_6);
    //MAP_PinDirModeSet(PIN_01,PIN_DIR_MODE_OUT);
    //MAP_PinConfigSet(PIN_01, PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);

    //
    // Configure PIN_02 for SDHost0 SDCARD_CMD
    //
    MAP_PinTypeSDHost(PIN_08, PIN_MODE_8);
    MAP_PinConfigSet(PIN_08,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);
    //MAP_PinTypeSDHost(PIN_02, PIN_MODE_6);
    //MAP_PinConfigSet(PIN_02,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);


    //
    // Enable MMCHS
    //
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST,PRCM_RUN_MODE_CLK);

    //
    // Reset MMCHS
    //
    MAP_PRCMPeripheralReset(PRCM_SDHOST);

    //
    // Configure MMCHS
    //
    MAP_SDHostInit(SDHOST_BASE);

    //
    // Configure card clock
    //
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST),1500000);


//    MAP_PinTypeGPIO(PIN_06, PIN_MODE_0, false);
//    MAP_GPIODirModeSet(GPIOA1_BASE, 0x80,GPIO_DIR_MODE_IN);
//    MAP_PinTypeGPIO(PIN_08, PIN_MODE_0, false);
//    MAP_GPIODirModeSet(GPIOA2_BASE, 0x2,GPIO_DIR_MODE_IN);

}
