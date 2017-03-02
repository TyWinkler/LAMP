//*****************************************************************************
//
// Application Name     - LED Driver for LPD8806 to work with the CC3200
// Application Overview - Allows for easy function calls to be used with LPD8806
// Application Details  - https://github.com/TyWinkler/LAMP/tree/master/SPIExample
// Authors              - Ty Winkler
//
//*****************************************************************************

//*****************************************************************************
// Pinmux config as follows
//
// Configure PIN_05 for SPI0 GSPI_CLK
// MAP_PinTypeSPI(PIN_05, PIN_MODE_7);
//
// Configure PIN_07 for SPI0 GSPI_MOSI
// MAP_PinTypeSPI(PIN_07, PIN_MODE_7);
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

#include "gpio.h"


// Global Variables
#define SPI_IF_BIT_RATE  5000
#define NUMLEDS 24

unsigned long pixels[NUMLEDS];

//*****************************************************************************
//
//! SPI Initialization
//!
//! This function configures SPI modelue for use with the LPD8806
//!
//! \return None.
//
//*****************************************************************************
void LPD8806Init(){

    // Enable the SPI module clock
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    // Reset the peripheral
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    // Reset SPI
    MAP_SPIReset(GSPI_BASE);

    // Configure SPI interface
    MAP_SPIConfigSetExpClk(GSPI_BASE,
                     MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,
                     SPI_MODE_MASTER,
                     SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS | SPI_3PIN_MODE | SPI_TURBO_OFF | SPI_CS_ACTIVELOW | SPI_WL_8));

    // Enable SPI for communication
    MAP_SPIEnable(GSPI_BASE);
}

//*****************************************************************************
//
//! reset
//!
//! Resets the LPD8806 so that its ready to receive a new set of inputs
//! This does not clear the strip
//!
//! \return None.
//
//*****************************************************************************
void reset(){
    unsigned int i;
    unsigned long ulDummy;

    // Enable SPI Transfer
    MAP_SPICSEnable(GSPI_BASE);

    // Transfer the 0x00 bytes to the board to get ready for a new send command
    for(i = 0; i < (3 * ((NUMLEDS + 63) / 64)); i++){
        MAP_SPIDataPut(GSPI_BASE,0x00);
        MAP_SPIDataGet(GSPI_BASE,&ulDummy);
    }

    MAP_SPICSDisable(GSPI_BASE);
}

//*****************************************************************************
//
//! display
//!
//! Sends the colors stored in the pixels array to the LPD8806
//! Be sure that the LPD8806 has been reset() before trying to display()
//! a new set of colors. display() will automatically prepare for a new send
//!
//! \return None.
//
//*****************************************************************************
void display(){
    unsigned int i;
    unsigned long ulDummy;

    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_0,1);  // pin 50 GPIO_PIN_0

    // Enable SPI Transfer
    MAP_SPICSEnable(GSPI_BASE);

    // Transfer the colors to the leds
    for(i = 0; i < NUMLEDS + 1; i++){
        MAP_SPIDataPut(GSPI_BASE,(pixels[i] | 0x80));
        MAP_SPIDataGet(GSPI_BASE,&ulDummy);
        MAP_SPIDataPut(GSPI_BASE,((pixels[i]>>8) | 0x80));
        MAP_SPIDataGet(GSPI_BASE,&ulDummy);
        MAP_SPIDataPut(GSPI_BASE,((pixels[i]>>16) | 0x80));
        MAP_SPIDataGet(GSPI_BASE,&ulDummy);
    }

    MAP_SPICSDisable(GSPI_BASE);

    // Prepare for a new transfer
    reset();

    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_0,0);
}

//*****************************************************************************
//
//! color
//!
//! Creates the hex color value based on 24 bit color (255,255,255)
//! Because the LPD8806 uses the first bit of every color value
//! the actual sent color is only on a scale of 1 to 127 value
//! this function automatically fixes the value to the correct color
//!
//! Note that the LPD8806 takes sorts colors by GRB
//!
//! \return unsigned long c.
//
//*****************************************************************************
unsigned long color(unsigned char r, unsigned char g, unsigned char b){
    unsigned long c;
    c = 0x808080 | (((unsigned long)g / 2) << 16)
                 | (((unsigned long)r / 2) << 8)
                 | ((unsigned long)b / 2);
    return c;
}

//*****************************************************************************
//
//! color
//!
//! Creates the hex color value based on 24 bit hex value
//! Because the LPD8806 uses the first bit of every color value
//! the actual sent color is only on a scale of 1 to 127 value
//! this function automatically fixes the value to the correct color
//!
//! Note that the LPD8806 takes sorts colors by GRB
//!
//! \return unsigned long c.
//
//*****************************************************************************
unsigned long colorHex(unsigned long hex){
    unsigned int r, g, b, c;
    g = (hex >> 8) & 0xFF;
    r = (hex >> 16) & 0xFF;
    b = (hex) & 0xFF;
    c = 0x808080 | (((unsigned long)g / 2) << 16)
                 | (((unsigned long)r / 2) << 8)
                 | ((unsigned long)b / 2);
    return c;
}

//*****************************************************************************
//
//! setPixel
//!
//! Sets a pixel in the array to preferred color value
//!
//! \return None.
//
//*****************************************************************************
void setPixel(unsigned int p, unsigned long color){
    if(p > NUMLEDS){
        return;
    } else {
        pixels[p] = color;
    }
}

//*****************************************************************************
//
//! allColor
//!
//! This functions sets the entire strip to the color given as an input
//!
//! \return None.
//
//*****************************************************************************
void allColor(unsigned long color){
    int i;
    for(i = 0; i < NUMLEDS; i++){
        setPixel(i,color);
    }
    display();
}

//*****************************************************************************
//
//! clear
//!
//! This functions turns off the led strip
//!
//! \return None.
//
//*****************************************************************************
void clearStrip(){
    allColor(0x000000);
}
