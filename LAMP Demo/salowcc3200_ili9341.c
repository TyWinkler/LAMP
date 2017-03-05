/*
 * salowCC3200_ili9341.c
 *
 *  Created on: Apr 10, 2016
 *      Author: salow
 */
#include "grlib.h"

#include "hw_gpio.h"
#include "hw_ints.h"
#include "hw_memmap.h"

#include "hw_types.h"
#include "gpio.h"
#include "interrupt.h"
#include "timer.h"
#include "rom.h"

#include "salowcc3200_ili9341.h"

#include "utils.h"
#include "prcm.h"
#include "spi.h"
#include "uart_if.h"

/////////////
#define ILI9340_TFTWIDTH  240
#define ILI9340_TFTHEIGHT 320

#define ILI9340_NOP     0x00
#define ILI9340_SWRESET 0x01
#define ILI9340_RDDID   0x04
#define ILI9340_RDDST   0x09

#define ILI9340_SLPIN   0x10
#define ILI9340_SLPOUT  0x11
#define ILI9340_PTLON   0x12
#define ILI9340_NORON   0x13

#define ILI9340_RDMODE  0x0A
#define ILI9340_RDMADCTL  0x0B
#define ILI9340_RDPIXFMT  0x0C
#define ILI9340_RDIMGFMT  0x0A
#define ILI9340_RDSELFDIAG  0x0F

#define ILI9340_INVOFF  0x20
#define ILI9340_INVON   0x21
#define ILI9340_GAMMASET 0x26
#define ILI9340_DISPOFF 0x28
#define ILI9340_DISPON  0x29

#define ILI9340_CASET   0x2A
#define ILI9340_PASET   0x2B
#define ILI9340_RAMWR   0x2C
#define ILI9340_RAMRD   0x2E

#define ILI9340_PTLAR   0x30
#define ILI9340_MADCTL  0x36


#define ILI9340_MADCTL_MY  0x80
#define ILI9340_MADCTL_MX  0x40
#define ILI9340_MADCTL_MV  0x20
#define ILI9340_MADCTL_ML  0x10
#define ILI9340_MADCTL_RGB 0x00
#define ILI9340_MADCTL_BGR 0x08
#define ILI9340_MADCTL_MH  0x04

#define ILI9340_PIXFMT  0x3A

#define ILI9340_FRMCTR1 0xB1
#define ILI9340_FRMCTR2 0xB2
#define ILI9340_FRMCTR3 0xB3
#define ILI9340_INVCTR  0xB4
#define ILI9340_DFUNCTR 0xB6

#define ILI9340_PWCTR1  0xC0
#define ILI9340_PWCTR2  0xC1
#define ILI9340_PWCTR3  0xC2
#define ILI9340_PWCTR4  0xC3
#define ILI9340_PWCTR5  0xC4
#define ILI9340_VMCTR1  0xC5
#define ILI9340_VMCTR2  0xC7

#define ILI9340_RDID1   0xDA
#define ILI9340_RDID2   0xDB
#define ILI9340_RDID3   0xDC
#define ILI9340_RDID4   0xDD

#define ILI9340_GMCTRP1 0xE0
#define ILI9340_GMCTRN1 0xE1
/*
#define ILI9340_PWCTR6  0xFC

*/

// Color definitions
#define ILI9340_BLACK   0x0000
#define ILI9340_BLUE    0x001F
#define ILI9340_RED     0xF800
#define ILI9340_GREEN   0x07E0
#define ILI9340_CYAN    0x07FF
#define ILI9340_MAGENTA 0xF81F
#define ILI9340_YELLOW  0xFFE0
#define ILI9340_WHITE   0xFFFF



#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/////////////////////

//*****************************************************************************
//
// This driver operates in four different screen orientations.  They are:
//
// * Portrait - The screen is taller than it is wide, and the flex connector is
//              on the bottom of the display.  This is selected by defining
//              PORTRAIT.
//
// * Landscape - The screen is wider than it is tall, and the flex connector is
//               on the right side of the display.  This is selected by
//               defining LANDSCAPE.
//
// * Portrait flip - The screen is taller than it is wide, and the flex
//                   connector is on the top of the display.  This is selected
//                   by defining PORTRAIT_FLIP.
//
// * Landscape flip - The screen is wider than it is tall, and the flex
//                    connector is on the left side of the display.  This is
//                    selected by defining LANDSCAPE_FLIP.
//
// These can also be imagined in terms of screen rotation; if portrait mode is
// 0 degrees of screen rotation, landscape is 90 degrees of counter-clockwise
// rotation, portrait flip is 180 degrees of rotation, and landscape flip is
// 270 degress of counter-clockwise rotation.
//
// If no screen orientation is selected, portrait mode will be used.
//
//*****************************************************************************
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT
#endif

//*****************************************************************************
//
// Defines for the pins that are used to communicate with the ILI934x.
//
//*****************************************************************************

//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00ff0000) >> 19) |               \
                                 ((((c) & 0x0000ff00) >> 5) & 0x000007e0) | \
                                 ((((c) & 0x000000ff) << 8) & 0x0000f800))
//*****************************************************************************
//
// salow                        CD CD RD WD
//
//*****************************************************************************
typedef void (*pfnWriteData)(unsigned short usData);
typedef void (*pfnWriteCommand)(unsigned char ucData);
typedef void (*pfnWriteDataH)(unsigned short usData);
//*****************************************************************************
//
// Function pointers for low level LCD controller access functions.
//
//*****************************************************************************

static void WriteDataGPIO(unsigned short usData);
static void WriteCommandGPIO(unsigned char ucData);
static void WriteDataHI(unsigned short usData);

pfnWriteDataH WriteDataH = WriteDataHI;
pfnWriteData WriteData = WriteDataGPIO;
pfnWriteCommand WriteCommand = WriteCommandGPIO;


void CommandEnable()
{
   //GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_7,0);  // pin 62 GPIO_PIN_7
   GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_6,0);  // pin 61 GPIO_PIN_7
}

void CommandDisable()
{
    //GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_7,GPIO_PIN_7);
    GPIOPinWrite(GPIOA0_BASE,GPIO_PIN_6,GPIO_PIN_6);
}

void ChipSelectEnable()
{
    //GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_4,0);  // pin3
    GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,0);  // pin 63
}

void ChipSelectDisable()
{
    //GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_4,GPIO_PIN_4);
    GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_0,GPIO_PIN_0);
}
void
WriteDataGPIO16(unsigned short usData)
{
    unsigned long ulDummy;

    //
    // Write the most significant byte of the data to the bus.
    //
    ChipSelectEnable();
    SPIDataPut(GSPI_BASE,(usData >> 8));
    SPIDataGet(GSPI_BASE,&ulDummy);
    ChipSelectDisable();

    //
    // Write the least significant byte of the data to the bus.
    //
    ChipSelectEnable();
    SPIDataPut(GSPI_BASE,(usData & 0xFF));
    SPIDataGet(GSPI_BASE,&ulDummy);
    ChipSelectDisable();

}

static void
WriteDataHI(unsigned short usData)
{
    unsigned long ulDummy;

    //
    // Write the most significant byte of the data to the bus.
    //
    ChipSelectEnable();
    SPIDataPut(GSPI_BASE,(usData >> 8));
    SPIDataGet(GSPI_BASE,&ulDummy);
    ChipSelectDisable();

    //
    // Write the least significant byte of the data to the bus.
    //
    ChipSelectEnable();
    SPIDataPut(GSPI_BASE,(usData & 0xFF));
    SPIDataGet(GSPI_BASE,&ulDummy);
    ChipSelectDisable();

}
static void
WriteDataGPIO(unsigned short usData)
{
    unsigned long ulDummy;

    //
    // Write the most significant byte of the data to the bus.
    //

    CommandDisable();
    ChipSelectEnable();
    SPIDataPut(GSPI_BASE,usData);
        SPIDataGet(GSPI_BASE,&ulDummy);
        //Report("regi %lld  \n\n\r", ulDummy);

     ChipSelectDisable();

    //
    // Write the least significant byte of the data to the bus.
    //
   /* ChipSelectEnable();
    SPIDataPut(GSPI_BASE,(usData & 0xFF));
    SPIDataGet(GSPI_BASE,&ulDummy);
    ChipSelectDisable();*/

}
static void
WriteCommandGPIO(unsigned char ucData)
{
    unsigned long ulDummy;

    //
    // Write the command to the bus.
    //

    ChipSelectEnable();
    CommandEnable();
    SPIDataPut(GSPI_BASE,ucData);
    SPIDataGet(GSPI_BASE,&ulDummy);
    //Report("regi %d, &ulDummy");
    ChipSelectDisable();
    CommandDisable();
}
static unsigned short
ReadData(void)
{
    unsigned long usData;

    ChipSelectEnable();

    //
    // Return the data that was read.
    //
    SPIDataGet (GSPI_BASE, &usData);
    ChipSelectDisable();

    return((unsigned short)usData);
}
static unsigned short
ReadRegister(unsigned char ucIndex)
{
    WriteCommand(ucIndex);
    return(ReadData());
}

//*****************************************************************************
//
// salow end
//
//*****************************************************************************
//*****************************************************************************
//
// Writes a data word to the ILI934x.
//
//*****************************************************************************

//*****************************************************************************
//
// Write a particular controller register with a value.
//
//*****************************************************************************
//static void
//WriteRegister(unsigned char ucIndex, unsigned short usValue)
//{
//    WriteCommand(ucIndex);
//    WriteData(usValue);
//}

//******************************************************************************
//
//Function which initialize the LCD screen.
//
//******************************************************************************

void
salowCC3200_ili9341InitScreen(void)
{
    unsigned long temp;
    temp = 80000000 / (3 * 1000);   //  temp=1 ms

    CommandDisable();
    ChipSelectDisable();

    WriteCommand(0x01);  //software reset

    //  Wait until the data has been transmitted
    //while(SSIBusy(SSI0_BASE))
    //{
    //}

    //SysCtlDelay(5 * temp);
    UtilsDelay(5*temp);
     //WriteCommand(0x28);   // display off

     WriteCommand(0xEF);
      WriteData(0x03);
      WriteData(0x80);
      WriteData(0x02);

      WriteCommand(0xCF);
      WriteData(0x00);
      WriteData(0XC1);
      WriteData(0X30);

      WriteCommand(0xED);
      WriteData(0x64);
      WriteData(0x03);
      WriteData(0X12);
      WriteData(0X81);

      WriteCommand(0xE8);
      WriteData(0x85);
      WriteData(0x00);
      WriteData(0x78);

      WriteCommand(0xCB);
      WriteData(0x39);
      WriteData(0x2C);
      WriteData(0x00);
      WriteData(0x34);
      WriteData(0x02);

      WriteCommand(0xF7);
      WriteData(0x20);

      WriteCommand(0xEA);
      WriteData(0x00);
      WriteData(0x00);

      WriteCommand(ILI9341_PWCTR1);    //Power control
      WriteData(0x23);   //VRH[5:0]

      WriteCommand(ILI9341_PWCTR2);    //Power control
      WriteData(0x10);   //SAP[2:0];BT[3:0]

      WriteCommand(ILI9341_VMCTR1);    //VCM control
      WriteData(0x3e); //�Աȶȵ���
      WriteData(0x28);

      WriteCommand(ILI9341_VMCTR2);    //VCM control2
      WriteData(0x86);  //--

      WriteCommand(ILI9341_MADCTL);    // Memory Access Control
      WriteData(0x48);

      WriteCommand(ILI9341_PIXFMT);
      WriteData(0x55);

      WriteCommand(ILI9341_FRMCTR1);
      WriteData(0x00);
      WriteData(0x18);

      WriteCommand(ILI9341_DFUNCTR);    // Display Function Control
      WriteData(0x08);
      WriteData(0x82);
      WriteData(0x27);

      WriteCommand(0xF2);    // 3Gamma Function Disable
      WriteData(0x00);

      WriteCommand(ILI9341_GAMMASET);    //Gamma curve selected
      WriteData(0x01);

      WriteCommand(ILI9341_GMCTRP1);    //Set Gamma
      WriteData(0x0F);
      WriteData(0x31);
      WriteData(0x2B);
      WriteData(0x0C);
      WriteData(0x0E);
      WriteData(0x08);
      WriteData(0x4E);
      WriteData(0xF1);
      WriteData(0x37);
      WriteData(0x07);
      WriteData(0x10);
      WriteData(0x03);
      WriteData(0x0E);
      WriteData(0x09);
      WriteData(0x00);

      WriteCommand(ILI9341_GMCTRN1);    //Set Gamma
      WriteData(0x00);
      WriteData(0x0E);
      WriteData(0x14);
      WriteData(0x03);
      WriteData(0x11);
      WriteData(0x07);
      WriteData(0x31);
      WriteData(0xC1);
      WriteData(0x48);
      WriteData(0x08);
      WriteData(0x0F);
      WriteData(0x0C);
      WriteData(0x31);
      WriteData(0x36);
      WriteData(0x0F);
      WriteCommand(ILI9340_SLPOUT);
      UtilsDelay(120 * temp);
      WriteCommand(ILI9340_DISPON);
    //UtilsDelay(100 * temp);



    UtilsDelay(100 * temp);

    WriteCommand(0x2c);     //  Memory Write
    //  This command is used to transfer data from MCU to frame memory. This command makes no change to the other driver
    //  status. When this command is accepted, the column register and the page register are reset to the Start Column/Start
    //  Page positions. The Start Column/Start Page positions are different in accordance with MADCTL setting.) Then D [17:0] is
    //  stored in frame memory and the column register and the page register incremented. Sending any other command can stop
    //  frame Write. X = Don�t care.


}


//*****************************************************************************
//
//! Initializes the display driver.
//!
//! This function initializes the ILI9341 display
//! controller on the panel, preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void
salowCC3200_ili9341Init(void)
{
    unsigned long ulClockMS;

    //
    // Get the current processor clock frequency.
    //
    ulClockMS = 80000000 / (3 * 1000);



    CommandDisable();
    ChipSelectDisable();

    //
    // Delay for 10ms.
    //
    UtilsDelay(10 * ulClockMS);

    salowCC3200_ili9341InitScreen();
}




//*****************************************************************************
//
//! Determines whether an ILI9341 controller is present.
//!
//! This function queries the ID of the display controller in use and returns
//! it to the caller.  This driver supports both ILI9341.
//! These are very similar but the sense of the long display axis is reversed
//! in the Formike KWH028Q02-F03 using an ILI9340 relative to the other two
//! supported displays and this information is needed by the touchscreen driver
//! to provide correct touch coordinate information.
//!
//! \return Returns 0x9340 if an ILI9341 controller is in use.
//
//*****************************************************************************
unsigned short
salowCC3200_ili9341ControllerIdGet(void)
{
    unsigned short usController;

    //
    // Determine which version of the display controller we are using.
    //
    usController = ReadRegister(0x00);

    return(usController);
}

//*****************************************************************************
//
//! Turns on the backlight.
//!
//! This function turns on the backlight on the display.
//!
//! \return None.
//
//*****************************************************************************
void
salowCC3200_ili9341BacklightOn(void)
{
    //
    // Assert the signal that turns on the backlight.
    //
    //HWREG(LCD_BL_BASE + GPIO_O_DATA + (LCD_BL_PIN << 2)) = LCD_BL_PIN;
    WriteCommand(0x53);     //  Write CTRL Display (53h)
    WriteData(0x24);
}

//*****************************************************************************
//
//! Turns off the backlight.
//!
//! This function turns off the backlight on the display.
//!
//! \return None.
//
//*****************************************************************************
void
salowCC3200_ili9341BacklightOff(void)
{
    //
    // Deassert the signal that turns on the backlight.
    //
    //HWREG(LCD_BL_BASE + GPIO_O_DATA + (LCD_BL_PIN << 2)) = 0;
    WriteCommand(0x53);     //  Write CTRL Display (53h)
    WriteData(0x00);
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341PixelDraw(void *pvDisplayData, long lX, long lY,
                                   unsigned long ulValue)
{
    //
    // Set the X address of the display cursor.
    //
    WriteCommand(ILI9340_CASET);
#ifdef PORTRAIT
    WriteDataH(lX);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - lY);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - lX);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(lY);
#endif

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(ILI9340_PASET);
#ifdef PORTRAIT
    WriteDataH(lY);
#endif
#ifdef LANDSCAPE
    WriteDataH(lX);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - lY);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - lX);
#endif

    //
    // Write the pixel value.
    //
    WriteCommand(ILI9340_RAMWR);
    WriteDataH(ulValue);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341PixelDrawMultiple(void *pvDisplayData, long lX,
                                           long lY, long lX0, long lCount,
                                           long lBPP,
                                           const unsigned char *pucData,
                                           const unsigned char *pucPalette)
{
    unsigned long ulByte;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(0x80);
#ifdef PORTRAIT
    WriteDataH(0x0030);
#endif
#ifdef LANDSCAPE
    WriteDataH(0x0028);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(0x0000);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(0x0018);
#endif

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(0x2A);
#ifdef PORTRAIT
    WriteDataH(lX);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - lY);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - lX);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(lY);
#endif

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(0x2B);
#ifdef PORTRAIT
    WriteDataH(lY);
#endif
#ifdef LANDSCAPE
    WriteDataH(lX);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - lY);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - lX);
#endif

    //
    // Write the data RAM write command.
    //
    WriteCommand(0x2C);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                    WriteData(((unsigned long *)pucPalette)[(ulByte >>
                                                             (7 - lX0)) & 1]);
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        WriteData(DPYCOLORTRANSLATE(ulByte));

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            WriteData(DPYCOLORTRANSLATE(ulByte));

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(unsigned long *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                WriteData(DPYCOLORTRANSLATE(ulByte));
            }

            //
            // The image data has been drawn.
            //
            break;
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341LineDrawH(void *pvDisplayData, long lX1, long lX2,
                                   long lY, unsigned long ulValue)
{
    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(0x80);
#ifdef PORTRAIT
    WriteDataH(0x0030);
#endif
#ifdef LANDSCAPE
    WriteDataH(0x0028);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(0x0000);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(0x0018);
#endif

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(0x2A);
#ifdef PORTRAIT
    WriteDataH(lX1);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - lY);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - lX1);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(lY);
#endif

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(0x2B);
#ifdef PORTRAIT
    WriteDataH(lY);
#endif
#ifdef LANDSCAPE
    WriteDataH(lX1);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - lY);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - lX1);
#endif

    //
    // Write the data RAM write command.
    //
    WriteCommand(0x2C);

    //
    // Loop through the pixels of this horizontal line.
    //
    while(lX1++ <= lX2)
    {
        //
        // Write the pixel value.
        //
        WriteDataH(ulValue);
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341LineDrawV(void *pvDisplayData, long lX, long lY1,
                                   long lY2, unsigned long ulValue)
{
    //
    // Set the cursor increment to top to bottom, followed by left to right.
    //
    WriteCommand(0x80);
#ifdef PORTRAIT
    WriteDataH(0x0038);
#endif
#ifdef LANDSCAPE
    WriteDataH(0x0020);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(0x0008);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(0x0010);
#endif

    //
    // Set the X address of the display cursor.
    //
    WriteCommand(0x2A);
#ifdef PORTRAIT
    WriteDataH(lX);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - lY1);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - lX);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(lY1);
#endif

    //
    // Set the starting Y address of the display cursor.
    //
    WriteCommand(0x2B);
#ifdef PORTRAIT
    WriteDataH(lY1);
#endif
#ifdef LANDSCAPE
    WriteDataH(lX);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - lY1);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - lX);
#endif

    //
    // Write the data RAM write command.
    //
    WriteCommand(0x2C);

    //
    // Loop through the pixels of this vertical line.
    //
    while(lY1++ <= lY2)
    {
        //
        // Write the pixel value.
        //
        WriteDataH(ulValue);
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341RectFill(void *pvDisplayData, const tRectangle *pRect,
                                  unsigned long ulValue)
{
    long lCount;

    //
    // Write the X extents of the rectangle.
    //
    WriteCommand(ILI9340_CASET);
#ifdef PORTRAIT
    WriteDataH(pRect->sXMin);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - pRect->sYMax);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - pRect->sXMax);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(pRect->sYMin);
#endif
    WriteCommand(0x51);
#ifdef PORTRAIT
    WriteDataH(pRect->sXMax);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - pRect->sYMin);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - pRect->sXMin);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(pRect->sYMax);
#endif

    //
    // Write the Y extents of the rectangle.
    //
    WriteCommand(0x52);
#ifdef PORTRAIT
    WriteDataH(pRect->sYMin);
#endif
#ifdef LANDSCAPE
    WriteDataH(pRect->sXMin);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - pRect->sYMax);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - pRect->sXMax);
#endif
    WriteCommand(0x53);
#ifdef PORTRAIT
    WriteDataH(pRect->sYMax);
#endif
#ifdef LANDSCAPE
    WriteDataH(pRect->sXMax);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - pRect->sYMin);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - pRect->sXMin);
#endif

    //
    // Set the display cursor to the upper left of the rectangle.
    //
    WriteCommand(0x2A);
#ifdef PORTRAIT
    WriteDataH(pRect->sXMin);
#endif
#ifdef LANDSCAPE
    WriteDataH(239 - pRect->sYMin);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(239 - pRect->sXMin);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(pRect->sYMin);
#endif
    WriteCommand(0x2B);
#ifdef PORTRAIT
    WriteDataH(pRect->sYMin);
#endif
#ifdef LANDSCAPE
    WriteDataH(pRect->sXMin);
#endif
#ifdef PORTRAIT_FLIP
    WriteDataH(319 - pRect->sYMax);
#endif
#ifdef LANDSCAPE_FLIP
    WriteDataH(319 - pRect->sXMax);
#endif

    //
    // Write the data RAM write command.
    //
    WriteCommand(0x2C);

    //
    // Loop through the pixels of this filled rectangle.
    //
    for(lCount = ((pRect->sXMax - pRect->sXMin + 1) *
                  (pRect->sYMax - pRect->sYMin + 1)); lCount >= 0; lCount--)
    {
        //
        // Write the pixel value.
        //
        WriteDataH(ulValue);
    }

    //
    // Reset the X extents to the entire screen.
    //
    WriteCommand(0x40);
    WriteDataH(0x0000);
    WriteCommand(0x41);
    WriteDataH(0x00ef);

    //
    // Reset the Y extents to the entire screen.
    //
    WriteCommand(0x42);
    WriteDataH(0x0000);
    WriteCommand(0x43);
    WriteDataH(0x013f);
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static unsigned long
salowCC3200_ili9341ColorTranslate(void *pvDisplayData,
                                        unsigned long ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the ILI934x
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void
salowCC3200_ili9341Flush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}

//*****************************************************************************
//
//! The graphics library display structure that describes the driver for the
//! F02, F03 or F05 variants of the Formike Electronic KWH028Q02 TFT panel with
//! ILI934x controllers.
//
//*****************************************************************************
const tDisplay g_ssalowCC3200_ili9341 =
{
    sizeof(tDisplay),
    0,
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
    240,
    320,
#else
    320,
    240,
#endif
    salowCC3200_ili9341PixelDraw,
    salowCC3200_ili9341PixelDrawMultiple,
    salowCC3200_ili9341LineDrawH,
    salowCC3200_ili9341LineDrawV,
    salowCC3200_ili9341RectFill,
    salowCC3200_ili9341ColorTranslate,
    salowCC3200_ili9341Flush
};

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
