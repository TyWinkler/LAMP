//*****************************************************************************
// rom_pin_mux_config.c
//
// configure the device pins for different signals
//
// Copyright (c) 2016, Texas Instruments Incorporated - http://www.ti.com/
// All rights reserved.
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

// This file was automatically generated on 3/6/2017 at 2:48:27 PM
// by TI PinMux version 4.0.1482
//
//*****************************************************************************

#include "pinmux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "sdhost.h"
//*****************************************************************************
void PinMuxConfig(void){

    // Enable Peripheral Clocks
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_I2S, PRCM_RUN_MODE_CLK);

    // LEDs
    MAP_PinTypeGPIO(PIN_50, PIN_MODE_0, false); // Configure PIN_50 for GPIO Output - Chip Select
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x1, GPIO_DIR_MODE_OUT);

    // LCD Screen
    MAP_PinTypeGPIO(PIN_59, PIN_MODE_0, false); // Configure PIN_59 for GPIO Output - Reset
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x10, GPIO_DIR_MODE_OUT);
    MAP_PinTypeGPIO(PIN_61, PIN_MODE_0, false); // Configure PIN_61 for GPIO Output - D/C
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x40, GPIO_DIR_MODE_OUT);
    MAP_PinTypeGPIO(PIN_63, PIN_MODE_0, false); // Configure PIN_63 for GPIO Output - Chip Select
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x1, GPIO_DIR_MODE_OUT);

    // SPI
    MAP_PinTypeSPI(PIN_05, PIN_MODE_7); // Configure PIN_05 for SPI0 GSPI_CLK
    MAP_PinTypeSPI(PIN_06, PIN_MODE_7); // Configure PIN_06 for SPI0 GSPI_MISO
    MAP_PinTypeSPI(PIN_07, PIN_MODE_7); // Configure PIN_07 for SPI0 GSPI_MOSI

    // Network
    MAP_PinTypeGPIO(PIN_58, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x8, GPIO_DIR_MODE_IN);
    MAP_PinConfigSet(PIN_58,PIN_STRENGTH_2MA|PIN_STRENGTH_4MA,PIN_TYPE_STD_PD);

    // Speakers
    MAP_PinTypeI2C(PIN_03, PIN_MODE_5); // Configure PIN_03 for I2C0 I2C_SCL
    MAP_PinTypeI2C(PIN_04, PIN_MODE_5); // Configure PIN_04 for I2C0 I2C_SDA
    MAP_PinTypeI2S(PIN_60, PIN_MODE_6); // Configure PIN_60 for McASP0 McASP0_McAXR1
    MAP_PinTypeI2S(PIN_45, PIN_MODE_6); // Configure PIN_45 for McASP0 McASP0_McAXR0
    MAP_PinTypeI2S(PIN_15, PIN_MODE_7); // Configure PIN_15 for McASP0 McASP0_McAFSX
    MAP_PinTypeI2S(PIN_53, PIN_MODE_2); // Configure PIN_53 for McASP0 McASP0_McACLK

    // SDCard Reader
    MAP_PinTypeSDHost(PIN_64, PIN_MODE_6); // Configure PIN_64 for SDHost0 SDCARD_DATA
    MAP_PinConfigSet(PIN_64,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on CMD
    MAP_PinTypeSDHost(PIN_01, PIN_MODE_6); // Configure PIN_01 for SDHost0 SDCARD_CLK
    MAP_PinDirModeSet(PIN_01,PIN_DIR_MODE_OUT); // Set the SD card clock as output pin
    MAP_PinTypeSDHost(PIN_02, PIN_MODE_6); // Configure PIN_02 for SDHost0 SDCARD_CMD
    MAP_PinConfigSet(PIN_02,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on data
    MAP_PRCMPeripheralReset(PRCM_SDHOST); // Reset MMCHS
    MAP_SDHostInit(SDHOST_BASE); // Configure MMCHS
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), 15000000); // Configure card clock
}
