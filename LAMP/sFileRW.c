/*
 * sFileRW.c
 *
 *  Created on: Feb 6, 2017
 *      Author: Ty
 */

#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"

//Common interface includes
#include "gpio_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "pinmux.h"

#define SL_MAX_FILE_SIZE        64L*1024L       /* 64KB file */
#define BUF_SIZE                2048
#define USER_FILE_NAME          "fs_demo.txt"

typedef enum{
    // Choosing this number to avoid overlap w/ host-driver's error codes
    FILE_ALREADY_EXIST = -0x7D0,
    FILE_CLOSE_ERROR = FILE_ALREADY_EXIST - 1,
    FILE_NOT_MATCHED = FILE_CLOSE_ERROR - 1,
    FILE_OPEN_READ_FAILED = FILE_NOT_MATCHED - 1,
    FILE_OPEN_WRITE_FAILED = FILE_OPEN_READ_FAILED -1,
    FILE_READ_FAILED = FILE_OPEN_WRITE_FAILED - 1,
    FILE_WRITE_FAILED = FILE_READ_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

unsigned char gaucCmpBuf[BUF_SIZE];
const unsigned char SineWave[16] = {4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3};

//*****************************************************************************
//
//!  This funtion includes the following steps:
//!  -open a user file for writing
//!  -write "Old MacDonalds" child song 37 times to get just below a 64KB file
//!  -close the user file
//!
//!  /param[out] ulToken : file token
//!  /param[out] lFileHandle : file handle
//!
//!  /return  0:Success, -ve: failure
//
//*****************************************************************************
long WriteFileToDevice(unsigned long *ulToken, long *lFileHandle)
{
    long lRetVal = -1;
    int iLoopCnt = 0;

    //
    //  create a user file
    //
    lRetVal = sl_FsOpen((unsigned char *)USER_FILE_NAME,
                FS_MODE_OPEN_CREATE(65536, \
                          _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
                        ulToken,
                        lFileHandle);
    if(lRetVal < 0)
    {
        //
        // File may already be created
        //
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);
    }
    else
    {
        //
        // close the user file
        //
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        if (SL_RET_CODE_OK != lRetVal)
        {
            ASSERT_ON_ERROR(FILE_CLOSE_ERROR);
        }
    }

    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen((unsigned char *)USER_FILE_NAME,
                        FS_MODE_OPEN_WRITE,
                        ulToken,
                        lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(FILE_OPEN_WRITE_FAILED);
    }

    //
    // write "Old MacDonalds" child song as many times to get just below a 64KB file
    //
    for (iLoopCnt = 0;
            iLoopCnt < (SL_MAX_FILE_SIZE / sizeof(SineWave));
            iLoopCnt++)
    {
        lRetVal = sl_FsWrite(*lFileHandle,
                    (unsigned int)(iLoopCnt * sizeof(SineWave)),
                    (unsigned char *)SineWave, sizeof(SineWave));
        if (lRetVal < 0)
        {
            lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
            ASSERT_ON_ERROR(FILE_WRITE_FAILED);
        }
    }

    //
    // close the user file
    //
    lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        ASSERT_ON_ERROR(FILE_CLOSE_ERROR);
    }

    return SUCCESS;
}

//*****************************************************************************
//
//!  This funtion includes the following steps:
//!    -open the user file for reading
//!    -read the data and compare with the stored buffer
//!    -close the user file
//!
//!  /param[in] ulToken : file token
//!  /param[in] lFileHandle : file handle
//!
//!  /return 0: success, -ve:failure
//
//*****************************************************************************
long ReadFileFromDevice(unsigned long ulToken, long lFileHandle)
{
    long lRetVal = -1;
    int iLoopCnt = 0;

    //
    // open a user file for reading
    //
    lRetVal = sl_FsOpen((unsigned char *)USER_FILE_NAME,
                        FS_MODE_OPEN_READ,
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(FILE_OPEN_READ_FAILED);
    }

    //
    // read the data and compare with the stored buffer
    //
    for (iLoopCnt = 0;
            iLoopCnt < (SL_MAX_FILE_SIZE / sizeof(SineWave));
            iLoopCnt++)
    {
        lRetVal = sl_FsRead(lFileHandle,
                    (unsigned int)(iLoopCnt * sizeof(SineWave)),
                     gaucCmpBuf, sizeof(SineWave));
        if ((lRetVal < 0) || (lRetVal != sizeof(SineWave)))
        {
            lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
            ASSERT_ON_ERROR(FILE_READ_FAILED);
        }

        lRetVal = memcmp(SineWave,
                         gaucCmpBuf,
                         sizeof(SineWave));
        if (lRetVal != 0)
        {
            ASSERT_ON_ERROR(FILE_NOT_MATCHED);
        }
    }

    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        ASSERT_ON_ERROR(FILE_CLOSE_ERROR);
    }

    return SUCCESS;
}
