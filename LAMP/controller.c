// Hardware & driverlib library includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "osi.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern unsigned char g_ucSpkrStartFlag;

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
    while(1){
        //g_ucSpkrStartFlag = 0;
        //osi_Sleep(5000);
        g_ucSpkrStartFlag = 1;
        osi_Sleep(10000);
    }
}
