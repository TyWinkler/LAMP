//*****************************************************************************
//
// Application Name     -   LAMP
// Application Overview -   This is the Light and Music Project by group 15 of
//                          the University of Texas Senior Design. LAMP is a
//                          alarm clock like device that utilize both LEDs and
//                          speakers to simulate a rising sun. The application
//                          is paired with an android or iphone app that will
//                          allow users to configure their LAMP as they see fit.
//                          in addition to the alarm functionality the device
//                          will function as a general atmosphere device by
//                          changing the devices light patterns and music.
//
// Application Details  -   https://github.com/TyWinkler/LAMP/tree/master/LAMP
// Devices Used         -   MCU:            CC3200
//                      -   LED Strip:      LPD8806
//                      -   SDCard Reader:  SparkFun SD/MMC Card Breakout
//                      -   LCD Screen:     Ili9341
//                      -   Audio:          TI SimpleLink Wi-Fi CC3200 Audio BoosterPack
//
// Application Version  -   0.0.1
//
//*****************************************************************************

#include <stdlib.h>
#include <string.h>

//SimpleLink includes
#include "simplelink.h"

// free-rtos/ ti_rtos includes
#include "osi.h"

// Hardware & DriverLib library includes.
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "i2s.h"
#include "udma.h"
#include "gpio.h"
#include "gpio_if.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "pin.h"
#include "utils.h"
#include "spi.h"


//Common interface includes
#include "common.h"
#include "udma_if.h"
#include "uart_if.h"
#include "i2c_if.h"

//App include
#include "pinmux.h"
#include "network.h"
#include "circ_buff.h"
#include "control.h"
#include "audioCodec.h"
#include "i2s_if.h"
#include "pcm_handler.h"

//SDCard reader
#include "sdhost.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
tCircularBuffer *pRxBuffer;
tUDPSocket g_UdpSock;
OsiTaskHandle g_SpeakerTask = NULL ;
OsiTaskHandle g_NetworkTask = NULL ;
OsiTaskHandle g_LEDTask = NULL;
OsiTaskHandle g_LCDTask = NULL;

#define OSI_STACK_SIZE          1024
#define SAMPLERATE              44100
#define SPI_IF_BIT_RATE         20000000
#define TIMER_FREQ              80000000

unsigned char g_loopback = 1;

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//******************************************************************************
//                    FUNCTION DECLARATIONS
//******************************************************************************
extern void Speaker( void *pvParameters );
//extern void Network( void *pvParameters );
extern void LED( void *pvParameters );
extern void LCD( void *pvParameters );


//*****************************************************************************
//
//! Application defined hook (or callback) function - the tick hook.
//! The tick interrupt can optionally call this
//!
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationTickHook( void ){}

//*****************************************************************************
//
//! Application defined hook (or callback) function - assert
//!
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vAssertCalled( const char *pcFile, unsigned long ulLine ){
    while(1){}
}

//*****************************************************************************
//
//! Application defined idle task hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationIdleHook( void ){}

//*****************************************************************************
//
//! Application provided stack overflow hook function.
//!
//! \param  handle of the offending task
//! \param  name  of the offending task
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask, signed char *pcTaskName){
    ( void ) pxTask;
    ( void ) pcTaskName;
    for( ;; );
}

void vApplicationMallocFailedHook(){
    while(1){}
}


void SPIInit(){
    // Reset SPI
    SPIReset(GSPI_BASE);

    // Configure SPI interface
    SPIConfigSetExpClk(GSPI_BASE,PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (
                     SPI_3PIN_MODE |
                     SPI_TURBO_OFF |

                     SPI_WL_8));

    // Enable SPI for communication
    SPIEnable(GSPI_BASE);
}


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void BoardInit(void){
    /* In case of TI-RTOS vector table is initialize by OS itself */
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

//******************************************************************************
//                            MAIN FUNCTION
//******************************************************************************
int main(){
    long lRetVal = -1;
    unsigned char   RecordPlay;

    BoardInit();

    PinMuxConfig(); // Pinmux Configuration
    SPIInit();

    //SDCARD
    MAP_PinDirModeSet(PIN_01,PIN_DIR_MODE_OUT); // Set the SD card clock as output pin
    MAP_PinConfigSet(PIN_02,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on data
    MAP_PinConfigSet(PIN_64,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on CMD
    //MAP_PinConfigSet(PIN_01,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU); // Enable Pull up on CMD

    InitTerm(); // Initialising the UART terminal
    ClearTerm(); // Clearing the Terminal.

    //SDCARD
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST,PRCM_RUN_MODE_CLK); // Enable MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST); // Reset MMCHS
    MAP_SDHostInit(SDHOST_BASE); // Configure MMCHS
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), 15000000); // Configure card clock

    // Initialising the I2C Interface
    lRetVal = I2C_IF_Open(1);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    RecordPlay = I2S_MODE_TX;
    g_loopback = 1;

    // Create RX Buffer
    if(RecordPlay & I2S_MODE_TX)
    {
        pRxBuffer = CreateCircularBuffer(PLAY_BUFFER_SIZE);
        if(pRxBuffer == NULL)
        {
            UART_PRINT("Unable to Allocate Memory for Rx Buffer\n\r");
            LOOP_FOREVER();
        }
    }

    // Configure Audio Codec
    AudioCodecReset(AUDIO_CODEC_TI_3254, NULL);
    AudioCodecConfig(AUDIO_CODEC_TI_3254, AUDIO_CODEC_16_BIT, SAMPLERATE, AUDIO_CODEC_STEREO, AUDIO_CODEC_SPEAKER_ALL, AUDIO_CODEC_MIC_NONE);
    AudioCodecSpeakerVolCtrl(AUDIO_CODEC_TI_3254, AUDIO_CODEC_SPEAKER_ALL, 55);

    // Initialize the Audio(I2S) Module
    AudioInit();

    // Initialize the DMA Module
    UDMAInit();
    if(RecordPlay & I2S_MODE_TX)
    {
       UDMAChannelSelect(UDMA_CH5_I2S_TX, NULL);
        SetupPingPongDMATransferRx(pRxBuffer);
    }

    // Setup the Audio In/Out
    lRetVal = AudioSetupDMAMode(DMAPingPongCompleteAppCB_opt, CB_EVENT_CONFIG_SZ, RecordPlay);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }    
    AudioCaptureRendererConfigure(AUDIO_CODEC_16_BIT, SAMPLERATE, AUDIO_CODEC_STEREO, RecordPlay, 1);

    // Start Audio Tx/Rx
    Audio_Start(RecordPlay);

    // Start the simplelink thread
    //lRetVal = VStartSimpleLinkSpawnTask(9);
    //if(lRetVal < 0)
    //{
    //    ERR_PRINT(lRetVal);
    //    LOOP_FOREVER();
    //}

    // Start the Network Task
    //lRetVal = osi_TaskCreate( Network, (signed char*)"NetworkTask",\OSI_STACK_SIZE, NULL, 1, &g_NetworkTask );
    //if(lRetVal < 0)
    //{
    //    ERR_PRINT(lRetVal);
    //    LOOP_FOREVER();
    //}

    // Start the Control Task
//    lRetVal = ControlTaskCreate();
//    if(lRetVal < 0)
//    {
//        ERR_PRINT(lRetVal);
//        LOOP_FOREVER();
//    }

    // Start the Speaker Task
    lRetVal = osi_TaskCreate( Speaker, (signed char*)"Speaker",OSI_STACK_SIZE, NULL, 1, &g_SpeakerTask );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    // Start the LED Task
    lRetVal = osi_TaskCreate( LED, (signed char*)"LED",OSI_STACK_SIZE, NULL, 2, &g_LEDTask );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    // Start the LCD Task
    lRetVal = osi_TaskCreate( LCD, (signed char*)"LCD",OSI_STACK_SIZE, NULL, 2, &g_LCDTask );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    osi_start(); // Start the task scheduler
}
