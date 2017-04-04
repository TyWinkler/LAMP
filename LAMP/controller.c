// Hardware & driverlib library includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "hw_types.h"
#include "hw_ints.h"
#include "osi.h"
#include "prcm.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern unsigned char g_ucSpkrStartFlag;
extern unsigned long currentColor;
time_t currentTime = 0;

struct tm ts;
char timeBuf[80];

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! LED Routine
//!
//! \param pvParameters     Parameters to the task's entry function
//!
//! \return None
//
//*****************************************************************************
void Controller( void *pvParameters ){
    PRCMRTCInUseSet();
    PRCMRTCSet(currentTime,0);

    while(1){
        PRCMRTCGet((unsigned long*)currentTime,0);
        ts = *localtime(&currentTime);
        strftime(timeBuf, sizeof(timeBuf), "%a %Y-%m-%d %H:%M", &ts);
        osi_Sleep(1000);
    }
}
