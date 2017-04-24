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
#include "LCD.h"
#include "prcm.h"

//Common interface includes
#include "common.h"
#include "udma_if.h"
#include "uart_if.h"
#include "i2c_if.h"

//App include
#include "pinmux.h"
#include "circ_buff.h"
#include "audioCodec.h"
#include "i2s_if.h"
#include "pcm_handler.h"

#include "ff.h"
//Network


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
tCircularBuffer *pRxBuffer;
OsiTaskHandle g_SpeakerTask = NULL ;
OsiTaskHandle g_HTTPServerTask = NULL ;
OsiTaskHandle g_LEDTask = NULL;
OsiTaskHandle g_LCDTask = NULL;
OsiTaskHandle g_ControllerTask = NULL;
OsiMsgQ_t g_ControlMsgQueue;

OsiSyncObj_t g_SpeakerSyncObj;
OsiSyncObj_t g_ControllerSyncObj;
OsiSyncObj_t g_NetworkSyncObj;
OsiSyncObj_t g_FatFSSyncObj;

#define OSI_STACK_SIZE          1024
#define SAMPLERATE              44100
#define SPI_IF_BIT_RATE         20000000
#define TIMER_FREQ              80000000
#define PLAY_BUFFER_SIZE        70*1024
#define PLAY_WATERMARK          50*1024

typedef struct
{
  //Queue_Elem _elem;
  P_OSI_SPAWN_ENTRY pEntry;
  void* pValue;
}tTxMsg;

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
extern void Controller( void *pvParameters );
extern void HTTPServerTask( void *pvParameters );

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

void configureAudio(){
    long lRetVal = -1;

    // Initialising the I2C Interface
    lRetVal = I2C_IF_Open(1);
    if(lRetVal < 0){
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    pRxBuffer = CreateCircularBuffer(PLAY_BUFFER_SIZE);
    if(pRxBuffer == NULL){
        LcdPrintf("Unable to Allocate Memory for Rx Buffer");
        LOOP_FOREVER();
    }

    // Configure Audio Codec
    AudioCodecReset(AUDIO_CODEC_TI_3254, NULL);
    AudioCodecConfig(AUDIO_CODEC_TI_3254, AUDIO_CODEC_16_BIT, SAMPLERATE, AUDIO_CODEC_STEREO, AUDIO_CODEC_SPEAKER_ALL, AUDIO_CODEC_MIC_NONE);
    AudioCodecSpeakerVolCtrl(AUDIO_CODEC_TI_3254, AUDIO_CODEC_SPEAKER_ALL, 60);

    // Initialize the Audio(I2S) Module
    AudioInit();

    // Initialize the DMA Module
    UDMAInit();

    UDMAChannelSelect(UDMA_CH5_I2S_TX, NULL);
    SetupPingPongDMATransferRx(pRxBuffer);

    // Setup the Audio In/Out
    lRetVal = AudioSetupDMAMode(DMAPingPongCompleteAppCB_opt, CB_EVENT_CONFIG_SZ, I2S_MODE_TX);
    if(lRetVal < 0){
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    AudioCaptureRendererConfigure(AUDIO_CODEC_16_BIT, SAMPLERATE, AUDIO_CODEC_STEREO, I2S_MODE_TX, 1);

    // Start Audio Tx/Rx
    Audio_Start(I2S_MODE_TX);
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

    BoardInit();

    PinMuxConfig(); // Pinmux Configuration

    SPIInit();
    LCDReset();
    displaymytext();
    PRCMRTCInUseSet();
    PRCMRTCSet(0,0);

    configureAudio();

    //Test
    osi_SyncObjCreate(&g_SpeakerSyncObj);
    osi_SyncObjCreate(&g_ControllerSyncObj);
    osi_SyncObjCreate(&g_NetworkSyncObj);
    osi_SyncObjCreate(&g_FatFSSyncObj);

    // Start the Controller Task
#ifdef CONTROLLER
    lRetVal = osi_TaskCreate( Controller, (signed char*)"Controller",OSI_STACK_SIZE, NULL, 1, &g_ControllerTask );
    if(lRetVal < 0){
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
#endif

#ifdef SPEAKER
    // Start the Speaker Task
    lRetVal = osi_TaskCreate( Speaker, (signed char*)"Speaker",OSI_STACK_SIZE, NULL, 3, &g_SpeakerTask );
    if(lRetVal < 0){
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
#endif

#ifdef NETWORK
    //
    // Start the simplelink thread
    //
    lRetVal = VStartSimpleLinkSpawnTask(9);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    lRetVal = osi_MsgQCreate(&g_ControlMsgQueue,"g_ControlMsgQueue", sizeof(tTxMsg),1);
    ASSERT_ON_ERROR(lRetVal);

    // Create HTTP Server Task
    lRetVal = osi_TaskCreate(HTTPServerTask, (signed char*)"HTTPServerTask", 4096, NULL, 2, &g_HTTPServerTask );
    if(lRetVal < 0){
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
#endif

    osi_start(); // Start the task scheduler
}
