//*****************************************************************************
// pinmux.c
//
// configure the device pins for different signals
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

//*****************************************************************************
void
PinMuxConfig(void)
{
    //
    // Set unused pins to PIN_MODE_0 with the exception of JTAG pins 16,17,19,20
    //

    //
    // Enable Peripheral Clocks 
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_I2S, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    //MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
   // MAP_PinTypeUART(PIN_57, PIN_MODE_3);

    //
    // Configure PIN_04 for GPIOInput
    //
    MAP_PinTypeGPIO(PIN_04, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_IN);

    //
    // Configure PIN_15 for GPIOInput
    //
    MAP_PinTypeGPIO(PIN_15, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_IN);

    //SDCard Init
    //
    // Configure PIN_06 for SDHOST0 SDHost_D0
    //
    MAP_PinTypeSDHost(PIN_06, PIN_MODE_8);

    //
    // Configure PIN_07 for SDHOST0 SDHost_CLK
    //
    MAP_PinTypeSDHost(PIN_07, PIN_MODE_8);

    //
    // Configure PIN_08 for SDHOST0 SDHost_CMD
    //
    MAP_PinTypeSDHost(PIN_08, PIN_MODE_8);
	
    //
    // Configure PIN_03 for I2C0 I2C_SCL			Brown
    //
    MAP_PinTypeI2C(PIN_03, PIN_MODE_5);
    //
    // Configure PIN_04 for I2C0 I2C_SDA			Green
    //
    MAP_PinTypeI2C(PIN_04, PIN_MODE_5);
    //
    // Configure PIN_60 for McASP0 McASP0_McAXR1	White/Grey
    //
    MAP_PinTypeI2S(PIN_60, PIN_MODE_6);
    //
    // Configure PIN_45 for McASP0 McASP0_McAXR0	Orange
    //
    MAP_PinTypeI2S(PIN_45, PIN_MODE_6);
    //
    // Configure PIN_15 for McASP0 McASP0_McAFSX	Yellow
    //
    MAP_PinTypeI2S(PIN_15, PIN_MODE_7);
    //
    // Configure PIN_53 for McASP0 McASP0_McACLK	Blue
    //
    MAP_PinTypeI2S(PIN_53, PIN_MODE_2);
	
}
