//*****************************************************************************
// network.c
//
// Network Interface
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

#include <stdio.h>
#include <stdlib.h>

// Simplelink includes
#include "simplelink.h"

//driverlib includes
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "gpio_if.h"
#include "uart_if.h"
//common interface includes
#include "common.h"
#include "LPD8806.h"
#include "LCD.h"
#include "api.h"

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************

#define ROLE_INVALID                    (-5)
#define AP_SSID_LEN_MAX                 (33)
#define SH_GPIO_9                       (9)            /* Red */
#define SH_GPIO_11                      (11)           /* Green */
#define SH_GPIO_25                      (25)           /* Yellow */
#define AUTO_CONNECTION_TIMEOUT_COUNT   (50)           /* 5 Sec */

#define IP_ADDR             0xc0a80064 /* 192.168.0.100 */
#define PORT_NUM            5001
#define BUF_SIZE            1400
#define TCP_PACKET_COUNT    1000

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,
    SOCKET_CREATE_ERROR = DEVICE_NOT_IN_STATION_MODE - 1,
    BIND_ERROR = SOCKET_CREATE_ERROR - 1,
    LISTEN_ERROR = BIND_ERROR -1,
    SOCKET_OPT_ERROR = LISTEN_ERROR -1,
    CONNECT_ERROR = SOCKET_OPT_ERROR -1,
    ACCEPT_ERROR = CONNECT_ERROR - 1,
    SEND_ERROR = ACCEPT_ERROR -1,
    RECV_ERROR = SEND_ERROR -1,
    SOCKET_CLOSE_ERROR = RECV_ERROR -1,
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_uiIpAddress = 0; //Device IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
char g_cBsdBuf[BUF_SIZE];

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'
            // Applications can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s , "
                       "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on application's "
                           "request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_uiIpAddress = pEventData->ip;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                       "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}


//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if(pDevEvent == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        return;
    }

    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status )
            {
                case SL_ECLOSE:
                    /*UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n",
                                    pSock->EventData.sd);*/
                    break;
                default:
                    /*UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->EventData.sd, pSock->EventData.status); */
                  break;
            }
            break;

        default:
           /*UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);*/
          break;
    }
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_uiIpAddress = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}

//****************************************************************************
//
//! Confgiures the mode in which the device will work
//!
//! \param iMode is the current mode of the device
//!
//!
//! \return   SlWlanMode_t
//!
//
//****************************************************************************
static int ConfigureMode(int iMode)
{
    long   lRetVal = -1;

    lRetVal = sl_WlanSetMode(iMode);
    ASSERT_ON_ERROR(lRetVal);

    /* Restart Network processor */
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);

    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);

    return sl_Start(NULL,NULL,NULL);
}


//*****************************************************************************
//
//! Connect the Device to Network
//!
//! \param  None
//!
//! \return  0 - Success
//!            -1 - Failure
//!
//*****************************************************************************

long ConnectToNetwork()
{
    long lRetVal = -1;
    //unsigned int uiConnectTimeoutCnt =0;

#ifdef DEBUG
    LcdPrintf("slStart");
#endif

    //Start Simplelink Device
    lRetVal =  sl_Start(NULL,NULL,NULL);
    ASSERT_ON_ERROR(lRetVal);

#ifdef DEBUG
    LcdPrintf("Started");
#endif

    if(lRetVal != ROLE_STA)
    {
        if (ROLE_AP == lRetVal)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
            }
        }
        //
        // Configure to STA Mode
        //
        lRetVal = ConfigureMode(ROLE_STA);
        if(lRetVal !=ROLE_STA)
        {
            //UART_PRINT("Unable to set STA mode...\n\r");
            lRetVal = sl_Stop(SL_STOP_TIMEOUT);
            CLR_STATUS_BIT_ALL(g_ulStatus);
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

//    //waiting for the device to Auto Connect
//    while(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT &&
//        ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))
//    {
//        //Turn Green LED On
//        GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
//        osi_Sleep(50);
//        //Turn Green LED Off
//        GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
//        osi_Sleep(50);
//
//        uiConnectTimeoutCnt++;
//    }
//    //Couldn't connect Using Auto Profile
//    if(uiConnectTimeoutCnt==AUTO_CONNECTION_TIMEOUT_COUNT)
//    {
//        CLR_STATUS_BIT_ALL(g_ulStatus);
//
//        //Turn Green LED On
//        GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
//
//        //Connect Using Smart Config
//        lRetVal = SmartConfigConnect();
//        ASSERT_ON_ERROR(lRetVal);
//
//        //Waiting for the device to Auto Connect
//        while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
//        {
//            MAP_UtilsDelay(500);
//        }
//
//        //Turn Green LED Off
//        GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
//    }

    SlSecParams_t secParams = {0};
    lRetVal = 0;

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

#ifdef DEBUG
    LcdPrintf("slWlan");
#endif

    lRetVal = sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

#ifdef DEBUG
    LcdPrintf("Connected");
#endif

    /* Wait */
    //while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    //{
        // Wait for WLAN Event
#ifndef SL_PLATFORM_MULTI_THREADED
        //_SlNonOsMainLoopTask();
#endif
    //}

    return SUCCESS;

}


//*********************************************
//
//
//This function is used for parsing the string defining necessary API calls
//  that is sent via TCP sockets
//The appropriate API calls are made within the function, assuming the expected
//  parameters are correctly received.
//The function returns a 1 if an API call was successfully made, but returns a
//  -1 for error if an API call was not successfully made
//
//
//*********************************************

int APIparse(char *commands){
    char *token= strtok(commands, "=");
#ifdef DEBUG
    clearScreen();
    LcdPrintf(token);
#endif
    if(strncmp(token, "api_call",8) != 0){
#ifdef DEBUG
        LcdPrintf("uhh?");
#endif
        return -1;
    }

    //takes api_call type (i.e. set_color, add_alarm, etc.)
    token= strtok(NULL, "&");
#ifdef DEBUG
    LcdPrintf(token);
#endif

    //SET COLOR
    if(strncmp(token, "set_color",9)==0){
        long color;
        token= strtok(NULL, "=");
#ifdef DEBUG
        LcdPrintf(token);
#endif
        if(strncmp(token, "color",5)==0){
            token=strtok(NULL, ""); //will this take the last token?
#ifdef DEBUG
            LcdPrintf(token);
#endif
            color=strtol(token + 2, NULL, 16) & 0x00FFFFFF;
#ifdef DEBUG
            char test[50];
            sprintf (test, "%x", color);
            LcdPrintf(test);
#endif
            apiSetColorIm(color);
            return 1;
        }
    }

    //ADD ALARM
    if(strncmp(token, "add_alarm",9)==0){
        long time= -1;
        int themeID = -1;
        int dow = -1;
        int alarmID = -1;
        int running = -1;

        token=strtok(NULL, "=");
#ifdef DEBUG
        LcdPrintf(token);
#endif
        //parse time
        if(strncmp(token, "time",4)==0){
            token=strtok(NULL, "&");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strncmp(token, "NA",2) != 0){
                time=strtol(token, NULL, 10);
            }

            //parse theme_id
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strncmp(token, "theme_id",8)==0){
                token= strtok(NULL, "&");
#ifdef DEBUG
                LcdPrintf(token);
#endif
                if(strncmp(token, "NA",2)!=0){
                    themeID = -1;
                    //token should just contain the one char if not NA
                }
            }

            //parse dow
            token = strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strncmp(token, "dow",3)==0){
                token=strtok(NULL, "&");
#ifdef DEBUG
                LcdPrintf(token);
#endif
                //what is dow? Why is it being sent as a string of bits, but passed as a char?
                //TODO: convert dow string to char
            }

            //parse alarm id
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strncmp(token, "alarm_id",8)==0){
                token= strtok(NULL, "&");
#ifdef DEBUG
                LcdPrintf(token);
#endif
                    alarmID = token[0];
                    //token should just contain the one char
            }

            //parse running
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strncmp(token, "running",7)==0){
                token= strtok(NULL, "&");   //last token, check
#ifdef DEBUG
                LcdPrintf(token);
#endif
                if(strncmp(token, "NA",2) != 0){
                    running= token[0];
                    //token should just contain the one char
                }
            }

            apiEditAlarm(time, themeID, dow, alarmID, running);
            return 1;

        }
    }

    //API OFF
    if(strncmp(token, "off",3)==0){
        apiOff();
        return 1;
    }

    //ADD THEME
    if(strcmp(token, "add_theme")==0){
        char themeID;
        long color;
        char song[30];
        char special;
        token=strtok(NULL, "=");
#ifdef DEBUG
                LcdPrintf(token);
#endif

        //get theme ID
        if(strcmp(token, "theme_id")==0){
            token=strtok(NULL, "&");
#ifdef DEBUG
                LcdPrintf(token);
#endif
            themeID=token[0] - 0x30;
            token=strtok(NULL, "=");
#ifdef DEBUG
                LcdPrintf(token);
#endif

            //take color value
            if(strcmp(token, "color")==0){
                token=strtok(NULL, "&");
#ifdef DEBUG
                LcdPrintf(token);
#endif
                color = strtol(token + 2, NULL, 16);
            }
            //take song name
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strcmp(token, "song")==0){
                token=strtok(NULL, "&");
#ifdef DEBUG
            LcdPrintf(token);
#endif
                strcpy(song,token);
                //PASSING NA FOR NOW FOR SONG
            }
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strcmp(token, "volume")==0){
                token=strtok(NULL, "&");
#ifdef DEBUG
            LcdPrintf(token);
#endif
                //don't know what to do with volume
                //token will hold string value of volume at this point
            }
            token=strtok(NULL, "=");
#ifdef DEBUG
            LcdPrintf(token);
#endif
            if(strcmp(token, "special")==0){
                token=strtok(NULL, "");
#ifdef DEBUG
            LcdPrintf(token);
#endif
                special= token[0];
                apiEditTheme(themeID, color, song);
                return 1;
            }
        }
    }

    //DELETE ALARM
    if(strcmp(token, "delete_alarm")==0){
        token=strtok(NULL, "=");
        char alarmID;
        if(strcmp(token, "alarm_id")==0){
            token=strtok(NULL, "&");
            alarmID=token[0];
            apiDeleteAlarm(alarmID);
            return 1;
        }

    }

    //DELETE THEME
    if(strcmp(token, "delete_theme")==0){
        token=strtok(NULL, "=");
        char themeID;
        if(strcmp(token, "theme_id")==0){
            token=strtok(NULL, "&");
            themeID=token[0] - 0x30;
            apiDeleteTheme(themeID);
            return 1;
        }
    }

    //PLAY THEME
    if(strcmp(token, "play_theme")==0){
        char themeID;
        token=strtok(NULL, "=");
        if(strcmp(token, "theme_id")==0){
            token=strtok(NULL, "&");
            themeID=token[0] - 0x30;
#ifdef DEBUG
            LcdPrintf("%d",themeID);
#endif
            apiPlayTheme(themeID);
            return 1;
        }

    }

    //SET TIME
    if(strcmp(token, "set_time")==0){
        token=strtok(NULL, "=");
        unsigned long time;
        if(strcmp(token, "time")==0){
            token=strtok(NULL, "&");
            time = strtol(token, NULL, 10);
            apiUpdateTime(time);
            return 1;
        }
    }

    return -1;
}

//****************************************************************************
//
//! \brief Opening a TCP server side socket and receiving data
//!
//! This function opens a TCP socket in Listen mode and waits for an incoming
//!    TCP connection.
//! If a socket connection is established then the function will try to read
//!    1000 TCP packets from the connected client.
//!
//! \param[in] port number on which the server will be listening on
//!
//! \return     0 on success, -1 on error.
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//
//****************************************************************************
int BsdTcpServer(unsigned short usPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iCounter;
    int             iAddrSize;
    int             iSockID;
    int             iStatus;
    int             iNewSockID;
    long            lNonBlocking = 1;
    int             iTestBufLen;

    // filling the buffer
    for (iCounter=0 ; iCounter<BUF_SIZE ; iCounter++)
    {
        g_cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    iTestBufLen  = BUF_SIZE;

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)usPort);
    sLocalAddr.sin_addr.s_addr = 0;

#ifdef DEBUG
    LcdPrintf("Creating socket");
#endif

    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( iSockID < 0 )
    {
        // error
        sl_Close(iSockID);
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
    }

    iAddrSize = sizeof(SlSockAddrIn_t);

    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, iAddrSize);
    if( iStatus < 0 )
    {
        // error
        sl_Close(iSockID);
        ASSERT_ON_ERROR(BIND_ERROR);
    }

#ifdef DEBUG
    clearScreen();
    LcdPrintf("Listening");
#endif

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if( iStatus < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(LISTEN_ERROR);
    }

#ifdef DEBUG
    LcdPrintf("Nonblocking");
#endif

    // setting socket option to make the socket as non blocking
    iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                            &lNonBlocking, sizeof(lNonBlocking));
    if( iStatus < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(SOCKET_OPT_ERROR);
    }
    iNewSockID = SL_EAGAIN;

    // waiting for an incoming TCP connection
    while( iNewSockID < 0 )
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
#ifdef DEBUG
        clearScreen();
        LcdPrintf("Waiting to connect");
#endif
        iNewSockID = sl_Accept(iSockID, ( struct SlSockAddr_t *)&sAddr,
                                (SlSocklen_t*)&iAddrSize);
        if( iNewSockID == SL_EAGAIN )
        {
            osi_Sleep(1000);
        }
        else if( iNewSockID < 0 )
        {
            // error
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            ASSERT_ON_ERROR(ACCEPT_ERROR);
        }
    }

    // waits for 1000 packets from the connected TCP client
    //while (lLoopCount < g_ulPacketCount)
    //{
    while(1){
        iStatus = sl_Recv(iNewSockID, g_cBsdBuf, iTestBufLen, 0);
        //------------------------------------------------------
        //
        // Smita code goes here!!!
        // g_cBsdBuf will contain the string array
        //
        //------------------------------------------------------
        //common.h needs to be set to your AP it should direct connect
#ifdef DEBUG
        clearScreen();
        LcdPrintf(g_cBsdBuf);
#endif
        APIparse(g_cBsdBuf);

        osi_Sleep(50);
        if( iStatus <= 0 )
        {
          // error
          sl_Close(iNewSockID);
          sl_Close(iSockID);
          break;
          //ASSERT_ON_ERROR(RECV_ERROR);
        }
    }
        //lLoopCount++;
    //}

    //Report("Recieved %u packets successfully\n\r",g_ulPacketCount);

    // close the connected socket after receiving from connected TCP client
    //iStatus = sl_Close(iNewSockID);
    //ASSERT_ON_ERROR(iStatus);
    // close the listening socket
    //iStatus = sl_Close(iSockID);
    //ASSERT_ON_ERROR(iStatus);

    return SUCCESS;
}

//*****************************************************************************
//
//! Network Task
//!
//! \param  pvParameters - Parameters to the task's entry function
//!
//! \return None
//!
//*****************************************************************************
void HTTPServerTask( void *pvParameters )
{
    long lRetVal = -1;

    //Initialize Global Variable
    InitializeAppVariables();

    //Connect to Network
    lRetVal = ConnectToNetwork();
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to establish connection w/ an AP \n\r");
        LOOP_FOREVER();
    }

#ifdef DEBUG
    LcdPrintf("Connected");
#endif

    while(1){
        lRetVal = BsdTcpServer(PORT_NUM);
        if(lRetVal < 0)
        {
            UART_PRINT("TCP Server failed\n\r");
            LOOP_FOREVER();
        }
        osi_Sleep(100);
    }

}
