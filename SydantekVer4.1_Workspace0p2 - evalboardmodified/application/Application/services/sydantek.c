/*
 * sydantek.c
 *
 * Copyright (c) 2020 and onwards, Carditek Medical Devices, Pvt. Ltd.
 * All rights reserved.
 */




/******************************************************************************

 @file       heart_rate.c

 @brief This file contains the Heart Rate sample application for use with the
        CC26xx Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************

 Copyright (c) 2011-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_ble_example_pack_01_50_00_62
 Release Date: 2017-11-01 10:38:41
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
///* POSIX Header files */
//#include "semaphore.h"
//#include "pthread.h"

//#include <unistd.h>



#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//#include <ti/sysbios/knl/Task.h>
//#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Event.h>
//#include <ti/sysbios/knl/Queue.h>
/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include <ti/display/Display.h>

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

//#include "peripheral.h"
#include "devinfoservice.h"


#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

#include <ti/drivers/GPIO.h>
#include <driverLib/timer.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <xdc/runtime/Error.h>
//#include <ti/sysbios/knl/Semaphore.h>
//#include <ti/ble5stack/boards/CC2640R2_LAUNCHXL/CC2640R2_LAUNCHXL.h>

#include <uartlog/UartLog.h>
#include <icall.h>
#include <icall_ble_api.h>
/* Application specific includes */

#include <services/sydantek.h>
#include <Battery_status.h>


#define UARTDUMP      0
#define UARTnBLEDump  1
#define BLEDump       0






// 251 - 3-byte ATT notification header, 4 byte L2CAP header.
#define PAYLOAD_BYTES         244

//typedef struct __attribute__((__packed__)) {
//    uint16_t version;  // has to be 1
//    uint16_t leads;    // 1, 2 or 8
//    uint16_t limb_hz;  // 500, 1000, 2000
//    uint16_t chest_hz; // 500, 1000, 2000
//} params_t;

typedef struct __attribute__((__packed__)) {
    uint16_t version;  // has to be 1
    uint16_t leads;    // 1, 2 or 8
    uint16_t limb_hz;  // 500, 1000, 2000
    uint16_t iBatvalue; // 500, 1000, 2000
    uint8_t  start_stop  // 0 is stop and 0xff start
} params_t;


#define SERVICE_INDEX         0
#define PARAMS_INDEX          1
#define NOTIFY_INDEX          2

static CONST uint8_t service_uuid[] = { SYDANTEK_UUID(SERVICE_INDEX) };
static CONST uint8_t params_uuid[] = { SYDANTEK_UUID(PARAMS_INDEX) };
static CONST uint8_t notify_uuid[] = { SYDANTEK_UUID(NOTIFY_INDEX) };

// Characteristic values & configuration.
static params_t params = { 1, 8, 1000, 1000 };
static uint8_t notify[1];  // not really used.
static gattCharCfg_t* notify_config;


int  iflag=1;

char fDRYCount;

#define SPIDMAABORT_T  20//which is 40 milisecond


//#define enabling  singal internal and external not enabled
#define INTERNAL_TEST_SIGNAL    0x10    //0x10 is test signal enabled
#define INTERNAL_TEST_SIGNAL_CH 0x15    //Internal Test Tone gain set 1

#define EXTERNAL_TEST_SIGNAL 0x00     //0x10 is test signal enabled



#define EXTERNAL_SIG         0x10  //external gain set 1
#define CH_GAIN              0x10


#define NOISE_MEASUREMENT    0x11  //CHnSET to  01 for noise Measurement cal
#define VOLTAGE_MEASUREMENT  0x13  //CHnSET Voltage Voltage Meaurement
#define TEMP_MEASUREMENT     0x14  //CHnSET Voltage TEMP Meaurement
#define CH_PDWN              0x80

#define CH2_P_RLD           0x06  // channel 2 positive to RLD Driver
#define CH2_N_RLD           0x07  // channel 2 positive to RLD Driver

#define RLD_AMP_ENABLED     0x40  //RLD Ampilfier enabled

int iSPIDMAAbortCnt;

//char cTempBfr[6][27]; //instaneously frames are loaded
uint8_t cTempBfr[8][30]; // channel on 1 -8 can be18 bytes to be collected to get Send the data
char cPenidingEvtf=0;
int iPacketNumber1Copy;

//static uint16  ECG_Sample_Count1=0,ECG_Sample_Count2=0,ECG_Sample_Count3=0;
//-------------------SLP 15 Feb 16
uint8 delay_count;
uint8 LED_State=1;
uint8 Board_SPI=0;


//GPIOCC26XX_Config
//GPIOCC26XX_config

PIN_Handle hCpuPins;
PIN_State pinState;


//PIN_Config gpiotable;
PIN_Config gpiotable[] = { PIN_TERMINATE };

PIN_Handle hEtagPins;
PIN_State  EtagPins;




UART_Params UART_Stting;                //slp 9 feb 16
UART_Handle     Uhandle;                 //slp 9 feb 16


//-----------------Hardware interrupt setup
Hwi_Handle hwiDRDY;
Hwi_Params hwiParams;
Error_Block eb;



//-----------------Software INterrupt Setup
Swi_Handle hswiDRDY;
Swi_Handle hSPIover;
Swi_Params swiParams;


uint8_t cTempbfr_tinkering[27];

uint8_t cIntrruptDistrub;


char        input;
//"Echoing characters:\r\n";
const uint8_t  echoPrompt[] = {0xff,0x1,0x2,0x3,0xff};
UART_Handle uart;
UART_Params uartParams;
uint16_t iReadbatCount=0;
uint8_t cBatlevelchange=1; // assuming the battery is high



//--------------------------


//uint8_t txBuf[27] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27};    // Transmit buffer
//uint8_t txBuf[27];
uint16_t Sampleindex=0;
uint8_t cTempTxbfr[27];
uint8_t cExternalbuffer[27];


uint16_t Sample_rate,Sample_rateCopy;


//uint8_t cTempRx[27];

uint16 Packet_Count;
uint16 Sample_data1;
uint16 Sample_data2;
uint16 Brpckt_Num;
uint16 delay1,delay2;
uint8 index=0;
uint16 Sim_Data=0;
uint8_t command_recieved_flag=0;
uint8_t AFE_Toggle=0;
uint8_t Frame_Pointer;          //SLP 16 Aug 16
uint8_t Buff_Change_Flag=0;     //SLP 17 Aug 16

uint8_t cSpiTxBuffer[30];
uint8_t cSpiRxBuffer1[30];
uint8_t cSpiRxBuffer2[30];
//uint8_t cExternalbuffer2[27];

//uint8_t cExternalbuffer3[244];
//uint8_t cExternalbuffer4[100];


SPI_Handle spiHandle;
uint8 SPI0=0;
uint8 SPI1=1;
uint8_t iSPIStatus;
int iPacketNumber=0;
int iPacketNumber1[8];

char cIDCountDisplay=0,cFailcount=0; //displayAfe ad1298 id to ble for 20 times
unsigned char cAFE_ID=0xff; //default ot 0xff
float f1,f2;


char iData_Not_Arrived; //if data  is not arrived in 50 m/s
unsigned int iDrdyCount=0;// for
static unsigned char xl=0;


//-----------
uint16_t iDataPointer;
int iInGetData,iInNextScanBuffer;
//------------

uint8_t NextScanBuffer(void);
inline void OneDataNotificationCorrected160bytes(void);
int AFEinit3(void);
int AFEinit4(void);
int AFEinit4_Changefrequecny(uint16_t iSampleRate);
int AFEinit4_Squarewave(uint16_t iSampleRate);



int SendtoAFE (SPI_Handle handle1, uint8 iSendCount,uint8 iRecvCount);
int GetDataFromScanBuffer(void);
/* Semaphore to block master until slave is ready for transfer */
sem_t masterSem;


static uint8_t cbleTxBfr_1[10][27];
static uint8_t cbleTxBfr_2[10][27];

static uint8_t cSelectbuffer=0,cbfrfilled=0x00;
static uint8_t cBufferCount=0;




PIN_Config BoardGpioInitTable1[]={


       CONFIG_PIN_4 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW  | PIN_PUSHPULL| PIN_DRVSTR_MED,//cs//11
                                        /* Parent Signal: AFE_RESET GPIO Pin, (DIO7) */
       CONFIG_PIN_5 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,//pwdn
                                        /* Parent Signal: AFE_PRDWN GPIO Pin, (DIO10) */

       CONFIG_PIN_6 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
                                         /* Parent Signal: AFE_PRDWN GPIO Pin, (DIO10) *///reset

       CONFIG_PIN_7 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED, //adsstart



       CONFIG_PIN_8 | PIN_INPUT_EN |PIN_NOPULL | PIN_IRQ_NEGEDGE, //DRDY

//       CONFIG_PIN_0| PIN_INPUT_EN |PIN_NOPULL ,


//       /* Parent Signal: CONFIG_UART_0 TX, (DIO12) */
//       CONFIG_PIN_4 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
//       /* Parent Signal: CONFIG_UART_0 RX, (DIO11) */
//       CONFIG_PIN_5 | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,




   PIN_TERMINATE
};


unsigned int ibugreport_1=0;
unsigned int ibugreport_2=0;
unsigned int ibugreport_3=0;
unsigned int ibugreport_4=0;


//---------------
/*********************************************************************
 * EXTERNAL VARIABLES
 */
 SPI_Handle      handle_spi;
 SPI_Params      params_spi;
 SPI_Transaction transaction;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * TYPEDEFS
 */


bool bDataReadinProgress;
unsigned int iPacketNumber2;
char chr0x33;
int iIntCount;
int  iSpiCount;
static uint8_t *ptrbuf,*ptrbuf1;
char cSpiTxferCmplete;
char cBleTxferCmplete;
void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId);


int iPacketNumberCopy=0;
char cfailCount=0;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;


static Clock_Struct battPerClock;

unsigned int cDebug;

/*********************************************************************
 * LOCAL FUNCTIONS
 */



void AFE_performPeriodicTask(void);
int ReadDataFromAFE(void);
int ReadDataFromAFEasynchronous(void);
int ReadDataFromAFEasynchronous_Debug(void);
int ReadDataFromAFEasynchronous_Debug_2(void);

//static void AFEinit_taskFxn(UArg a0, UArg a1);


volatile void swiDRDYFunction();
int AFEinit(void);
int AFEinit1(void);
int SendtoAFE2 (uint8 iSendCount,uint8 iRecvCount);
int AFEinit2(void);
//int SendtoAFE (SPI_HANDLE uint8 iSendCount,uint8 iRecvCount)
int cInterruptNumber;
unsigned char cInterprocessSem=0;
//extern hciStatus_t HCI_EXT_HaltDuringRfCmd( uint8 mode );
strScanBuffer ScanBuffer;


static uint8_t attribute_index(const gattAttrType_t* type) {
    if (type->len != ATT_UUID_SIZE) return 0xff;

    // All SYDANTEK_UUID bytes except the LSB should be identical.
    if (memcmp(type->uuid + 1, service_uuid + 1, ATT_UUID_SIZE - 1) != 0)
        return 0xff;
    return type->uuid[0];
}

static bStatus_t ReadAttribute(uint16_t handle, gattAttribute_t *attribute,
                               uint8_t *value, uint16_t *length,
                               uint16_t offset, uint16_t max_length,
                               uint8_t method)
{
    switch (attribute_index(&attribute->type)) {
    case PARAMS_INDEX:
        if (offset >= sizeof(params_t))
            return ATT_ERR_INVALID_OFFSET;
        *length = MIN(max_length, sizeof(params_t) - offset);
        memcpy(value, attribute->pValue + offset, *length);
        return SUCCESS;
    default:
        return ATT_ERR_ATTR_NOT_FOUND;
    }
}

static bStatus_t WriteAttribute(uint16_t handle, gattAttribute_t *attribute,
                                uint8_t *value, uint16_t length,
                                uint16_t offset, uint8_t method)
{
    if (attribute->type.len == ATT_BT_UUID_SIZE
            && *(uint16_t*) attribute->type.uuid == GATT_CLIENT_CHAR_CFG_UUID) {
        return GATTServApp_ProcessCCCWriteReq(
                handle, attribute, value, length, offset, GATT_CLIENT_CFG_NOTIFY);

    }

    switch (attribute_index(&attribute->type)) {
    case PARAMS_INDEX:
        if (offset >= sizeof(params_t))
            return ATT_ERR_INVALID_OFFSET;
        length = MIN(length, sizeof(params_t) - offset);
        memcpy(attribute->pValue + offset, value, length);
        return SUCCESS;
    default:
        return ATT_ERR_ATTR_NOT_FOUND;
    }
}

static gattAttribute_t attributes[] =
{
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0,
        (uint8_t*) &(gattAttrType_t) { ATT_UUID_SIZE, service_uuid }
    },
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &(uint8) { GATT_PROP_READ | GATT_PROP_WRITE }
    },
    {
        { ATT_UUID_SIZE, params_uuid },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *) &params
    },
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &(uint8) { GATT_PROP_NOTIFY | GATT_PROP_WRITE_NO_RSP }
    },
    {
        { ATT_UUID_SIZE, notify_uuid },
        GATT_PERMIT_WRITE,
        0,
        notify
    },
    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *) &notify_config
    },
};

static CONST gattServiceCBs_t callbacks =
        { ReadAttribute, WriteAttribute, NULL };

bStatus_t Sydantek_AddService(uint8_t rspTaskId)
{
    uint8_t status = SUCCESS;

    notify_config = (gattCharCfg_t *) ICall_malloc(
            sizeof(gattCharCfg_t) * linkDBNumConns);
    if (notify_config == NULL)
        return bleMemAllocError;
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, notify_config);

    status = GATTServApp_RegisterService(
            attributes, GATT_NUM_ATTRS(attributes), GATT_MAX_ENCRYPT_KEY_SIZE,
            &callbacks);
    Log_info2("Registered service, %d attributes, status=%d",
              GATT_NUM_ATTRS(attributes), status);
    return status;
}





/**--------------------------------------------------------------
void AFEChipSelect(void)
Selcts teh Active Low Chip select Pin of teh AFE
input: none
returns:nothing
----------------------------------------------------------------*/
void AFEChipSelect(void)
{
    PIN_setOutputValue(hCpuPins,11,0);
    //PIN_setOutputValue(hCpuPins,AFE_CS_EVALBOARD,0);
}

/**----------------------------------------------------------------
void AFEChipSelect(void)
DeSelcts teh Active Low Chip select Pin of the AFE
input: none
returns:nothing
----------------------------------------------------------------*/
void AFEChipUnSelect(void)
{
    //
     //cs=11
     //pwdn==22
     //reset=21
     //start==15



    PIN_setOutputValue(hCpuPins,11,1);
    //PIN_setOutputValue(hCpuPins,AFE_CS_EVALBOARD,1);
}

/**----------------------------------------------------------------
void ConnectPowertoAFE(void)
Enables 3.3V output by controlling BOard 3.3V enable pin on CPUGenerates
When this ppin is made 0, 3.3V is enabled
input: none
returns:nothing
----------------------------------------------------------------*/
void ConnectPowertoAFE(void)
{
        //keep digital bits low on power on
    //PIN_setOutputValue(hCpuPins,CONFIG_PIN_3,1);//CONFIG_SPI_0 MOSI

    //PIN_setOutputValue(hCpuPins,CONFIG_PIN_1,1);//CONFIG_SPI_0 SCLK
    //PIN_setOutputValue(hCpuPins,CONFIG_PIN_12,1);//CONFIG_SPI_0 CS

    //PIN_setOutputValue(hCpuPins,3,1);//CONFIG_SPI_0 CSAFE_CS
    //PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CSAFE_CS

    PIN_setOutputValue(hCpuPins,9,0);//CONFIG_SPI_0 MOSI
    PIN_setOutputValue(hCpuPins,10,0);//CONFIG_SPI_0 SCLK
    PIN_setOutputValue(hCpuPins,11,0);//CONFIG_SPI_0 CS
    //PIN_setOutputValue(hCpuPins,Board_3V3_EN,1);        //3.3 V is to kept enabled for AFE to work
}
/**---------------------------------------------------------------
void Delayus(int iDelay)
Routine to give so many us delay
input: int iDelay -- numner of uS
returns:nothing
----------------------------------------------------------------!*/
void DelayuS(int iDelay)
{
 uint32_t i,j,k;
for (i=0,j=0,k=0;i<=iDelay;i++){
    j=i*8;
    k=k+j;
}
}


//-------------------------------------------------------------------
//static void KeepPinsReady(void)
//--------------------------------------------------------------------
void KeepPinsReady(void)
{
    //PIN_Close(&pinState);
    //hCpuPins= PIN_open(&pinState,gpiotable);
    //hCpuPins= PIN_open(&pinState,BoardGpioInitTable1);


}


/**----------------------------------------------------------------
void ResetAFE(void)
Generates Hadr=ware Reset signal for AFE
input: none
returns:nothing
----------------------------------------------------------------*/
void HardwareResetAFE(void)
{
//    KeepPinsReady();
//    ConnectPowertoAFE();
//
//    PIN_setOutputValue(hCpuPins,AFE_RESET,0);
//    DelayuS(100);
//    PIN_setOutputValue(hCpuPins,AFE_RESET,1);
//    DelayuS(100);


        KeepPinsReady();
        ConnectPowertoAFE();
        PIN_setOutputValue(hCpuPins,21,0);

        DelayuS(100000);
        PIN_setOutputValue(hCpuPins,21,1);
        DelayuS(100000);


}
/**-------------------------------------------------------------
void AFEstart(void)
High-low-High pulse on Start line starts the ADC
input: none
returns:nothing
----------------------------------------------------------------*/
void AFEstart(void)
{
    //
     //cs=11
     //pwdn==22
     //reset=21
     //start==15


    PIN_setOutputValue(hCpuPins,15,0);       // AFE start is doen by sending command
                                                    // For command to be accepted, this line is to be kept low
    DelayuS(1);
    //PIN_setOutputValue(hCpuPins,AFE_START,1);
    //DelayuS(100);
    cSpiTxBuffer[0] = AFE_OPCODE_START;
    SendtoAFE (handle_spi,1,0);                                 //restart continuous read

}


/**----------------------------------------------------------------------
 * void SendtoAFE2 (uint8 iCount, uint8* ptr)
 * Rountien send number of bytes to AFE
 * input: iCount: no of bytes to be sent
 *  returns: AFE_TRANSFER_ERR(-1) if transfer counld not take palce
 *          else, no. of bytes that have bene received as reply
 *          (0 or more- depends on iRecv COunt specified)
 ----------------------------------------------------------------------*/
int SendtoAFE2 (uint8 iSendCount,uint8 iRecvCount)
{
    int i;
    bool b1;

    uint8_t cTempTx[100],cTempRx[100];


    for (i=0;i<iSendCount+iRecvCount;i++){
        cTempTx[i]=0x00;
        cTempRx[i]=0xFF;
    }
    for (i=0;i<iSendCount;i++){
        cTempTx[i]=cSpiTxBuffer[i];
    }

    transaction.count = iSendCount+iRecvCount;
    transaction.txBuf = cTempTx;
    transaction.rxBuf = cTempRx;

    b1=SPI_transfer(handle_spi, &transaction);
    if (!b1) {
        return AFE_TRANSFER_ERR;                       // returns error condition
    }

    for (i=0;i<iRecvCount;i++){
        cSpiRxBuffer1[i]=cTempRx[i+iSendCount]; // ignore the response of MISO for sent bytes and consider only received bytes
    }
    cSpiRxBuffer1[i]=cTempRx[i];
    cAFE_ID=cSpiRxBuffer1[0];
    cAFE_ID=cSpiRxBuffer1[0]; //for breakpoint

    //---------------
    return iRecvCount;                          // no. of bytes that has been received
}

/**----------------------------------------------------------------------
 * void SendtoAFE (uint8 iCount, uint8* ptr)
 * Rountien send number of bytes to AFE
 * input: iCount: no of bytes to be sent
 *  returns: AFE_TRANSFER_ERR(-1) if transfer counld not take palce
 *          else, no. of bytes that have bene received as reply
 *          (0 or more- depends on iRecv COunt specified)
 ----------------------------------------------------------------------*/
int SendtoAFE (SPI_Handle handle1, uint8 iSendCount,uint8 iRecvCount)
{
int i;
bool b1;

    uint8_t cTempTx[100],cTempRx[100];
    for (i=0;i<iSendCount+iRecvCount;i++){
        cTempTx[i]=0x00;
        cTempRx[i]=0xFF;
    }
    for (i=0;i<iSendCount;i++){
        cTempTx[i]=cSpiTxBuffer[i];
    }

    transaction.count = iSendCount+iRecvCount;
    transaction.txBuf = cTempTx;
    transaction.rxBuf = cTempRx;

    b1=SPI_transfer(handle_spi, &transaction);
    if (!b1) return AFE_TRANSFER_ERR;                       // returns error condition

    for (i=0;i<iRecvCount;i++){
        cSpiRxBuffer1[i]=cTempRx[i+iSendCount]; // ignore the response of MISO for sent bytes and consider only received bytes
    }
    cSpiRxBuffer1[i]=cTempRx[i];
    //---------------
    return iRecvCount;                          // no. of bytes that has been received
}


/**----------------------------------------------------------------
void ResetScanBuffer(void)
Used to initialise scan buffer
input: none
returns: nothing
 -----------------------------------------------------------------*/
void ResetScanBuffer(void)
{
    ScanBuffer.iIptr=0;         // both in and out pointers are pointing to beginning of scan buffer
    ScanBuffer.iOptr=0;
    ScanBuffer.iOverrunFlag=0;
    ScanBuffer.iCount=0;
}


/**-----------------------------reset the afe-------------------------------*/
void ResetAFE(void)
{

    PIN_setOutputValue(hCpuPins,22,1);
    PIN_setOutputValue(hCpuPins,21,1);
    DelayuS(100);
    PIN_setOutputValue(hCpuPins,21,0);
    DelayuS(1000);
    PIN_setOutputValue(hCpuPins,21,1);
    DelayuS(100);

}

/**---------------------------------------------------------------
uint8_t* NextScanBuffer(void)
points to 27 byte buffer in scan buffer into which data has to e put next
Uses ScanBuffer.iIptr as index
Increments the iIPtr index until it reaches  iOptr
input: none
returns:return 1 if buffer is found. return 0 if buffer is not found. the buffer address is retunred in global variable ptr buf
 ----------------------------------------------------------------*/
uint8_t NextScanBuffer(void)
{
 uint8_t * ptr,bfrlength;
static int q,r,s;
    q=0;
    ptr=0;
    //if (iInGetData==1) return 0;
    iInNextScanBuffer=1;
    q=ScanBuffer.iIptr;
    ptr=&(ScanBuffer.ScanData[q].cOneScanData[0]);
    r=ScanBuffer.iCount;
    bfrlength=SCAN_BUF_SIZE;
    if (r>bfrlength) {
        //k=1;
        iInNextScanBuffer=0;
        ScanBuffer.iOverrunFlag=1;
        ibugreport_3=r;
        return 0;
    }

    if (r<0) {
        ResetScanBuffer();
        q=ScanBuffer.iIptr;
        ptr=&(ScanBuffer.ScanData[q].cOneScanData[0]);
        ptrbuf=ptr;
        ScanBuffer.iOverrunFlag=2;
        ibugreport_4=r;
        iInNextScanBuffer=0;
        return 1;
    }

    r++;
    ScanBuffer.iOverrunFlag=0;
    iInNextScanBuffer=0;
    ptrbuf=ptr;

    return 1;
}


/**-----------------------------------------------------------------------------------
void swiDRDYFunction()
Call back fucntion gets control every time DRDY interrupt is generated,
HWI interrupt servcie routine is executed, which posts the SWI *
 -------------------------------------------------------------------------------------*/
volatile void swiDRDYFunction()
{
char ch=0;
    //ReadDataFromAFEasynchronous();

    ch=ReadDataFromAFEasynchronous_Debug_2();
    if(ch==2)
     {
        cIntrruptDistrub=1;
     }
    else{
        cIntrruptDistrub=0;
    }

    //ReadDataFromAFEasynchronous_Debug_2();

}


/**----------------------------------------------------------------
static void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId)
Call back function - gets control on DRDY interrupt
input: none
returns:nothing
----------------------------------------------------------------*/
void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId)
{

    //iDrdyCount++;;
    Swi_post(hswiDRDY);
}




/**----------------------------------------------------------------
Void spiCallback (SPI_Handle handle, SPI_Transaction *transaction)

------------------------------------------------------------------*/
void SPICallbackFunctionxx(SPI_Handle handle_spi, SPI_Transaction *pTransaction)
{

    //void *ptr;
    int m,n,o,i,k;
    uint8_t ch;
    uint8_t *ptr;
    unsigned char bfrlength=0,ch1=0;



//    if (transaction.status==SPI_TRANSFER_COMPLETED){
//
//
//        cSpiTxferCmplete=1 ;// indicate that process buffer increment is in processs
//        chr0x33= (iSpiCount & 0xff);
//        j=ScanBuffer.iCount;
//        if (j<SCAN_BUF_SIZE) {
//            i=ScanBuffer.iIptr; //move in pointer forward
//            i++;
//            if (i>=SCAN_BUF_SIZE) i=0;
//             ScanBuffer.iIptr=i;
//             ScanBuffer.iCount++;
//        }
//        cSpiTxferCmplete=0; //// indicates that process buffer increment is compeleted
//        //Event_post(syncEvent, 1);
//    } else {
//        chr0x33=0x44;
//    }

    if (transaction.status==SPI_TRANSFER_COMPLETED){


        cSpiTxferCmplete=1 ;// indicate that process buffer increment is in processs
        chr0x33= (iSpiCount & 0xff);



        bfrlength=SCAN_BUF_SIZE;
        m=ScanBuffer.iCount;
//        if (r>bfrlength) {
//            //k=1;
//            iInNextScanBuffer=0;
//            ScanBuffer.iOverrunFlag=1;
//            ibugreport_3=r;
//            return 0;
//        }


        if (m<0) {
            ResetScanBuffer();
            n=ScanBuffer.iIptr;

        }



        if (m<(bfrlength)) {
           // NextScanBuffer();

            n=ScanBuffer.iIptr;

            for(i=0;i<27;i++){
                ch=cExternalbuffer[i];
               ScanBuffer.ScanData[n].cOneScanData[i]=ch;            //returns required address in global variable ptrbuf
               //ScanBuffer.ScanData[n].cOneScanData[i]=ch;            //returns required address in global variable ptrbuf

            }

             //move in pointer forward
            n++;
            if (n>=(bfrlength)){
                n=0;
            }
             ScanBuffer.iIptr=n;
             m++;
             ScanBuffer.iCount=m; // interupt overlaying
             cIntrruptDistrub=0;

        }
        cSpiTxferCmplete=0; //// indicates that process buffer increment is compeleted
        //Event_post(syncEvent, 1);
    } else {
        chr0x33=0x44;
    }

    //sem_post(&masterSem);


    // PIN_setOutputValue(hCpuPins,3,1);

}



/**----------------------------------------------------------------
Void spiCallback (SPI_Handle handle, SPI_Transaction *transaction)

------------------------------------------------------------------*/
void SPICallbackFunction(SPI_Handle handle_spi, SPI_Transaction *pTransaction)
{

    //void *ptr;
    int m,n,o,i,k;
    uint8_t ch;
    uint8_t *ptr;
    uint8_t bfrlength=0,ch1=0;



    if (transaction.status==SPI_TRANSFER_COMPLETED){
            cSpiTxferCmplete=1 ;// indicate that process buffer increment is in processs
            for(i=0;i<27;i++){
                ch=cExternalbuffer[i];
                if(cSelectbuffer==0){
                    cbleTxBfr_1[cBufferCount][i]=ch;
                }else{
                    if(cSelectbuffer==1){
                       cbleTxBfr_2[cBufferCount][i]=ch;
                    }
                }

            }
            cBufferCount++;
            if(cBufferCount>=10){
                if(cSelectbuffer==0){
                    cSelectbuffer=1;
                    cbfrfilled =1;
                }else{
                    cSelectbuffer=0;
                    cbfrfilled =2;
                }
                cBufferCount=0;
            }
            //returns required address in global variable ptrbuf
               //ScanBuffer.ScanData[n].cOneScanData[i]=ch;            //returns required address in global variable ptrbuf

    } else {
        chr0x33=0x44;
    }


}




/**----------------------------------------------------------------------
 * int ReadDataFromAFEasynchronous(void)
 * Read the
 * input: iCount: no of bytes to be sent
 * returns: AFE_TRANSFER_ERR(-1) if transfer counld not take palce
 *          else, no. of bytes that have bene received as reply
 *          (0 or more- depends on iRecv COunt specified)
 ----------------------------------------------------------------------*/
int ReadDataFromAFEasynchronous_Debug_2(void)
{
int i;
uint8_t *ptr2RxBuf,j,k,m;

    bool b1;

    for (i=0;i<AFE_DATA_LENGTH;i++){
        cTempTxbfr[i]=0xaa;
    }



    memset((void *) cExternalbuffer, 0, AFE_DATA_LENGTH);
    transaction.count = AFE_DATA_LENGTH;
    transaction.txBuf = (void *) cTempTxbfr;
    transaction.rxBuf = (void *) cExternalbuffer;


    transaction.arg=&iIntCount;
    b1=SPI_transfer(handle_spi, &transaction);
    //k=b1;
    //sleep(3);

    if(!b1){
    //PIN_setInterrupt(hCpuPins, AFE_DRDY|PIN_IRQ_NEGEDGE);
        return AFE_TRANSFER_ERR;                        // returns error condition
    }
    return  2;//AFE_DATA_LENGTH;                         // no. of bytes that has been received
}




//////////////////////////////////////////////////////////////////////////////////////////
/**----------------------------------------------------------------
void AFEinit(void)
Initialises AFE
input: none
returns:AFE_INIT_ERROR(0) if Init not succesful, AFE_INIT_SUCCESS(1) if succesful
----------------------------------------------------------------*/
int AFEinit3(void)
{
int i,j,ch;

// clear the content
for(j=0;j<10;j++){
    for(i=0;i<27;i++){
        cbleTxBfr_1[j][i]=0x00;
        cbleTxBfr_2[j][i]=0x00;
    }
}
i=0;
// test rouitne to check the gpio
PIN_init(BoardGpioInitTable1);
hCpuPins= PIN_open(&pinState,BoardGpioInitTable1);
//return 1;



//
// 1
//
////deBUG
//
////PIN_setOutputValue(hCpuPins, AFE_CS,0);//CONFIG_SPI_0 MOSI
////PIN_setOutputValue(hCpuPins,1,0);//CONFIG_SPI_0 SCLK
////PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CS
//
//cPenidingEvtf=PIN_registerIntCb(hCpuPins,DRDY_Callback);
//
//PIN_setConfig(hCpuPins, PIN_BM_IRQ,4  | PIN_IRQ_NEGEDGE);
//PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
//deBUG




SPI_init();

//-------SPI initalisation-----------
SPI_Params_init(&params_spi);                       // initialises parameters to defaults
params_spi.bitRate  = 500000;
params_spi.transferTimeout=100;
params_spi.transferMode=SPI_MODE_BLOCKING;              //SPI_MODE_CALLBACK;
//params.transferCallbackFxn=SPICallbackFunction;
params_spi.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
params_spi.mode        = SPI_MASTER;
handle_spi = SPI_open(0, &params_spi);              // initialise SPI


//handle_spi;
spiHandle=handle_spi;
bDataReadinProgress=0;


if (!spiHandle) {
    iSPIStatus=0;
    return AFE_INIT_ERROR;
} else {
    iSPIStatus=1;                                       // MARK SPI open is succesful
}


HardwareResetAFE();
ResetAFE();
DelayuS(10);
AFEChipSelect();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_STOP;
cSpiTxBuffer[1] = AFE_OPCODE_SDATAC;
cSpiTxBuffer[2] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[3] = 0;                             // we need to read one ID register
cSpiTxBuffer[4] = 0xFF;
i=SendtoAFE2 (4,1);                             //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
if (cSpiRxBuffer1[0]!=AFE_DEFINED_ID){
    return AFE_INIT_ERROR;
}

DelayuS(10);

//-------------------------------------------------------------------------------------------------------------------------

//--------------------configuration1 AS PER THE DATASHEET ------------------------


cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2(1,0);

cSpiTxBuffer[0] = AFE_OPCODE_RESET;
i=SendtoAFE2(1,0);
DelayuS(10); //internal voltage startup time

i=0;
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0xCC;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

//cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
DelayuS(15000); //internal voltage startup time=150ms

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

i=0;
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x85;// high frequency mode
//cSpiTxBuffer[i++] = 0x04;// high frequency mode

//cSpiTxBuffer[i++] = 0x85;
                                    //D2:0 data output rate
                                        //06=250 SPS,04=1KSPS,03=2kSPS
                                        // D8=0 LP mode, 1 HR mode
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG2;
cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] =0x00;                  //NO_INTERNAL_TEST_SIGNAL;
cSpiTxBuffer[i++] =0x00;       //0xx000             //INTERNAL_TEST_SIGNAL ON;                       //D4=INT_TEST=1 generates internal etst signal, =0 no test signal
                                        //D1:0=00 or 01 decides freq of test signal
                                        // D2=amplitude 0 lower, 1 higher amplitude

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

//cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF;
//cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] = 0xF3;
//                                        //RLDREF signalis internal
//i=SendtoAFE2 (i,40);
//
//i=0;
//cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
//cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
//cSpiTxBuffer[2] = 0xFF;
//i=SendtoAFE2 (2,40);
//i=0;
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH1SET;           //   D6:4 PGA gain .
                                                            //      000=gain 6
                                                            //      001=gain 1
                                                            //      010=gain 2
                                                            //      011=gain 3
                                                            //      100=gain 4
                                                            //      101=gain 8
                                                            //      110=gain 12

                                                            // D2:0 =00 external signal
                                                            //      =001input shorted
                                                            //      =101 test signal

cSpiTxBuffer[i++] =0x00;
//cSpiTxBuffer[i++] = 0x20;  // 0x10;  //external signal channel
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;////VOLTAGE_MEASUREMENT; //NOISE_MEASUREMENT;  // internal signal
i=SendtoAFE2 (i,20);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


#if Jorjin
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x65;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;


#else

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;

#endif





cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH4SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;;//NOISE_MEASUREMENT; //noise  masurement
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] =AFE_OPCODE_WREG | AFE_CH5SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x10;//NOISE_MEASUREMENT|CH_PDWN;;//|CH_PDWN;//NOISE_MEASUREMENT;//internal test signal CH_GAIN|CH2_P_RLD

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH6SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT; EXTERNAL_SIG//NOISE_MEASUREMENT;//internal test signal
                                                // D6:4 = GAIN..
                                                // 000=6,001=1,010=2,011=3,100=4,101=8,110=12
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH7SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH8SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;// NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH;;//EXTERNAL_SIG; //internal test signal                                  //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);


i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


////////////////////////////////////////////////////////////////////////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x00; //wct1 connected to ra lead1 channel1=0x09  //chanel2=0x0b                     //
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT2;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x00;// LA ann LL

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

i=0;
//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////
#if Jorjin


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
#else
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);


//////////////////////////////tAKEN FROM EVAL BAORD//////////////////////////////////////////////////////




#endif
//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////


//cSpiTxBuffer[i] = 0xFF;
//i=SendtoAFE2 (i,0);                               //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
ResetScanBuffer();

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);


cSpiTxBuffer[0] = AFE_OPCODE_STOP;
i=SendtoAFE2 (1,0);

cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2 (1,0);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);



ResetScanBuffer();
cSpiTxferCmplete=0;

DelayuS(10);
DelayuS(10);
DelayuS(10);

Error_init(&eb);
Swi_Params_init(&swiParams);


hswiDRDY = Swi_create((ti_sysbios_knl_Swi_FuncPtr)swiDRDYFunction, &swiParams,&eb);     // this sofwtare interrupt will be generated by DRDY HWI

if (hswiDRDY == NULL) {
    return AFE_INIT_ERROR;                          // eror occured while creating drdry function

}

//---------------------------------------------------------

AFEstart();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
SendtoAFE (handle_spi,1,0);                                 //restart continuous read

AFEChipUnSelect();
DelayuS(10);
AFEChipSelect();

SPI_close(handle_spi);
SPI_init();
SPI_Params_init(&params_spi);
params_spi.frameFormat =SPI_POL0_PHA1;             // this is Motorola format
params_spi.bitRate  =4000000;  //4mhz
params_spi.transferTimeout=1000;
params_spi.mode        = SPI_MASTER;
params_spi.transferMode=SPI_MODE_CALLBACK;//SPI_MODE_BLOCKING
params_spi.transferCallbackFxn=SPICallbackFunction;
handle_spi = SPI_open(SPI0, &params_spi);               // initialise SPI

DelayuS(10);
cInterprocessSem=0;



//hCpuPins= PIN_open(&pinState,& Pin);

cPenidingEvtf=PIN_registerIntCb(hCpuPins, DRDY_Callback);
////i=cPenidingEvtf;
//
//
//
PIN_setConfig(hCpuPins, PIN_BM_IRQ, 4   | PIN_IRQ_NEGEDGE);
PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
ResetScanBuffer();

return AFE_INIT_SUCCESS;

}




//////////////////////////////////////////////////////////////////////////////////////////
/**----------------------------------------------------------------
void AFEinit(void)
DES: dESC
input: none
returns:AFE_INIT_ERROR(0) if Init not succesful, AFE_INIT_SUCCESS(1) if succesful
----------------------------------------------------------------*/
int AFEinit4(void)
{
int i,j,ch;

// clear the content
for(j=0;j<10;j++){
    for(i=0;i<27;i++){
        cbleTxBfr_1[j][i]=0x00;
        cbleTxBfr_2[j][i]=0x00;
    }
}
i=0;
// test rouitne to check the gpio
PIN_init(BoardGpioInitTable1);
hCpuPins= PIN_open(&pinState,BoardGpioInitTable1);
//return 1;



//
// 1
//
////deBUG
//
////PIN_setOutputValue(hCpuPins, AFE_CS,0);//CONFIG_SPI_0 MOSI
////PIN_setOutputValue(hCpuPins,1,0);//CONFIG_SPI_0 SCLK
////PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CS
//
//cPenidingEvtf=PIN_registerIntCb(hCpuPins,DRDY_Callback);
//
//PIN_setConfig(hCpuPins, PIN_BM_IRQ,4  | PIN_IRQ_NEGEDGE);
//PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
//deBUG
SPI_init();

//-------SPI initalisation-----------
SPI_Params_init(&params_spi);                       // initialises parameters to defaults
params_spi.bitRate  = 500000;
params_spi.transferTimeout=100;
params_spi.transferMode=SPI_MODE_BLOCKING;              //SPI_MODE_CALLBACK;
//params.transferCallbackFxn=SPICallbackFunction;
params_spi.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
params_spi.mode        = SPI_MASTER;
handle_spi = SPI_open(0, &params_spi);              // initialise SPI


//handle_spi;
spiHandle=handle_spi;
bDataReadinProgress=0;


if (!spiHandle) {
    iSPIStatus=0;
    return AFE_INIT_ERROR;
} else {
    iSPIStatus=1;                                       // MARK SPI open is succesful
}



HardwareResetAFE();
ResetAFE();
DelayuS(10);
AFEChipSelect();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_STOP;
cSpiTxBuffer[1] = AFE_OPCODE_SDATAC;
cSpiTxBuffer[2] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[3] = 0;                             // we need to read one ID register
cSpiTxBuffer[4] = 0xFF;
i=SendtoAFE2 (4,1);                             //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
if (cSpiRxBuffer1[0]!=AFE_DEFINED_ID){
    return AFE_INIT_ERROR;
}

DelayuS(10);

//-------------------------------------------------------------------------------------------------------------------------

//--------------------configuration1 AS PER THE DATASHEET ------------------------


cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2(1,0);

cSpiTxBuffer[0] = AFE_OPCODE_RESET;
i=SendtoAFE2(1,0);
DelayuS(10); //internal voltage startup time

i=0;
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0xcc;                                   ;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

//cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
DelayuS(15000); //internal voltage startup time=150ms

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

i=0;
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x85;
;// high frequency mode
//cSpiTxBuffer[i++] = 0x04;// high frequency mode

//cSpiTxBuffer[i++] = 0x85;
                                    //D2:0 data output rate
                                        //06=250 SPS,04=1KSPS,03=2kSPS
                                        // D8=0 LP mode, 1 HR mode
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG2;
cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] =0x00;                  //NO_INTERNAL_TEST_SIGNAL;
cSpiTxBuffer[i++] =0x00;       //0xx000             //INTERNAL_TEST_SIGNAL ON;                       //D4=INT_TEST=1 generates internal etst signal, =0 no test signal
                                        //D1:0=00 or 01 decides freq of test signal
                                        // D2=amplitude 0 lower, 1 higher amplitude

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;
                                        //RLDREF signalis internal
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH1SET;           //   D6:4 PGA gain .
                                                            //      000=gain 6
                                                            //      001=gain 1
                                                            //      010=gain 2
                                                            //      011=gain 3
                                                            //      100=gain 4
                                                            //      101=gain 8
                                                            //      110=gain 12

                                                            // D2:0 =00 external signal
                                                            //      =001input shorted
                                                            //      =101 test signal

cSpiTxBuffer[i++] =0x00;
//cSpiTxBuffer[i++] = 0x20;  // 0x10;  //external signal channel
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;////VOLTAGE_MEASUREMENT; //NOISE_MEASUREMENT;  // internal signal
i=SendtoAFE2 (i,20);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


#if Jorjin
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x65;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;


#else

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;

#endif





cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH4SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//for rld meaurement
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] =AFE_OPCODE_WREG | AFE_CH5SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x10;//NOISE_MEASUREMENT|CH_PDWN;;//|CH_PDWN;//NOISE_MEASUREMENT;//internal test signal CH_GAIN|CH2_P_RLD

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH6SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT; EXTERNAL_SIG//NOISE_MEASUREMENT;//internal test signal
                                                // D6:4 = GAIN..
                                                // 000=6,001=1,010=2,011=3,100=4,101=8,110=12
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH7SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH8SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;// NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH;;//EXTERNAL_SIG; //internal test signal                                  //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);


i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////
#if Jorjin


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
#else
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x06;//0x6//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x06; //0x02   ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//////////////////////////////tAKEN FROM EVAL BAORD//////////////////////////////////////////////////////

//disable resipration ckt
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG4; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//disable resipration ckt
//LOFF_SENSP
//LOFF_SENSN
//LOFF_FLIP
//LOFF_STATP
//LOFF_STATN
//GPIO
//PACE
//RESP

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_FLIP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


/////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_GPIO; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



//////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_RESP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

#endif
//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x0b; //wct1 connected to ra lead1 channel1=0x09  //chanel2=0x0b                     //
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT2;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0xd4;// LA ann LL

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

//cSpiTxBuffer[i] = 0xFF;
//i=SendtoAFE2 (i,0);                               //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
ResetScanBuffer();

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);


cSpiTxBuffer[0] = AFE_OPCODE_STOP;
i=SendtoAFE2 (1,0);

cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2 (1,0);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);



ResetScanBuffer();
cSpiTxferCmplete=0;

DelayuS(10);
DelayuS(10);
DelayuS(10);

Error_init(&eb);
Swi_Params_init(&swiParams);


hswiDRDY = Swi_create((ti_sysbios_knl_Swi_FuncPtr)swiDRDYFunction, &swiParams,&eb);     // this sofwtare interrupt will be generated by DRDY HWI

if (hswiDRDY == NULL) {
    return AFE_INIT_ERROR;                          // eror occured while creating drdry function

}

//---------------------------------------------------------

AFEstart();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
SendtoAFE (handle_spi,1,0);                                 //restart continuous read

AFEChipUnSelect();
DelayuS(10);
AFEChipSelect();

SPI_close(handle_spi);
SPI_init();
SPI_Params_init(&params_spi);
params_spi.frameFormat =SPI_POL0_PHA1;             // this is Motorola format
params_spi.bitRate  =1000000;  //4mhz
params_spi.transferTimeout=1000;
params_spi.mode        = SPI_MASTER;
params_spi.transferMode=SPI_MODE_CALLBACK;//SPI_MODE_BLOCKING
params_spi.transferCallbackFxn=SPICallbackFunction;
handle_spi = SPI_open(SPI0, &params_spi);               // initialise SPI

DelayuS(10);
cInterprocessSem=0;



//hCpuPins= PIN_open(&pinState,& Pin);

cPenidingEvtf=PIN_registerIntCb(hCpuPins, DRDY_Callback);
////i=cPenidingEvtf;
//
//
//
PIN_setConfig(hCpuPins, PIN_BM_IRQ, 12   | PIN_IRQ_NEGEDGE);
PIN_setInterrupt(hCpuPins, 12|PIN_IRQ_NEGEDGE);
ResetScanBuffer();

return AFE_INIT_SUCCESS;

}



//////////////////////////////////////////////////////////////////////////////////////////
/**----------------------------------------------------------------
void AFEinit(void)
DES: dESC
input: none
returns:AFE_INIT_ERROR(0) if Init not succesful, AFE_INIT_SUCCESS(1) if succesful
----------------------------------------------------------------*/
int AFEinit4_Changefrequecny(uint16_t iSamplerate)
{
uint16_t   iSamplerateChange;
int i=0,j=0,ch=0;

iSamplerateChange=iSamplerate;

// clear the content
for(j=0;j<10;j++){
    for(i=0;i<27;i++){
        cbleTxBfr_1[j][i]=0x00;
        cbleTxBfr_2[j][i]=0x00;
    }
}
i=0;
// test rouitne to check the gpio
PIN_init(BoardGpioInitTable1);
hCpuPins= PIN_open(&pinState,BoardGpioInitTable1);
//return 1;



//
// 1
//
////deBUG
//
////PIN_setOutputValue(hCpuPins, AFE_CS,0);//CONFIG_SPI_0 MOSI
////PIN_setOutputValue(hCpuPins,1,0);//CONFIG_SPI_0 SCLK
////PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CS
//
//cPenidingEvtf=PIN_registerIntCb(hCpuPins,DRDY_Callback);
//
//PIN_setConfig(hCpuPins, PIN_BM_IRQ,4  | PIN_IRQ_NEGEDGE);
//PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
//deBUG




SPI_init();

//-------SPI initalisation-----------
SPI_Params_init(&params_spi);                       // initialises parameters to defaults
params_spi.bitRate  = 500000;
params_spi.transferTimeout=100;
params_spi.transferMode=SPI_MODE_BLOCKING;              //SPI_MODE_CALLBACK;
//params.transferCallbackFxn=SPICallbackFunction;
params_spi.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
params_spi.mode        = SPI_MASTER;
handle_spi = SPI_open(0, &params_spi);              // initialise SPI


//handle_spi;
spiHandle=handle_spi;
bDataReadinProgress=0;


if (!spiHandle) {
    iSPIStatus=0;
    return AFE_INIT_ERROR;
} else {
    iSPIStatus=1;                                       // MARK SPI open is succesful
}


HardwareResetAFE();
ResetAFE();
DelayuS(10);
AFEChipSelect();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_STOP;
cSpiTxBuffer[1] = AFE_OPCODE_SDATAC;
cSpiTxBuffer[2] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[3] = 0;                             // we need to read one ID register
cSpiTxBuffer[4] = 0xFF;
i=SendtoAFE2 (4,1);                             //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
if (cSpiRxBuffer1[0]!=AFE_DEFINED_ID){
    return AFE_INIT_ERROR;
}

DelayuS(10);

//-------------------------------------------------------------------------------------------------------------------------

//--------------------configuration1 AS PER THE DATASHEET ------------------------


cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2(1,0);

cSpiTxBuffer[0] = AFE_OPCODE_RESET;
i=SendtoAFE2(1,0);
DelayuS(10); //internal voltage startup time


if(iSamplerateChange==250){
    i=0;
    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] =0x4C;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    //cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
    DelayuS(15000); //internal voltage startup time=150ms

    cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
    cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
    cSpiTxBuffer[2] = 0xFF;
    i=SendtoAFE2 (2,40);

    i=0;

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x86;
}else{

    i=0;
    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] =0xCC;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    //cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
    DelayuS(15000); //internal voltage startup time=150ms

    cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
    cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
    cSpiTxBuffer[2] = 0xFF;
    i=SendtoAFE2 (2,40);

    i=0;

}

if(iSamplerateChange==500){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x86;
}// high frequency mode


if(iSamplerateChange==1000){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x85;
}// high frequency mode

if(iSamplerateChange==2000){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x84;
}// high frequency mode

//cSpiTxBuffer[i++] = 0x04;// high frequency mode

//cSpiTxBuffer[i++] = 0x85;
                                    //D2:0 data output rate
                                        //06=250 SPS,04=1KSPS,03=2kSPS
                                        // D8=0 LP mode, 1 HR mode
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG2;
cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] =0x00;                  //NO_INTERNAL_TEST_SIGNAL;
cSpiTxBuffer[i++] =0x00;       //0xx000             //INTERNAL_TEST_SIGNAL ON;                       //D4=INT_TEST=1 generates internal etst signal, =0 no test signal
                                        //D1:0=00 or 01 decides freq of test signal
                                        // D2=amplitude 0 lower, 1 higher amplitude

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;
                                        //RLDREF signalis internal
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH1SET;           //   D6:4 PGA gain .
                                                            //      000=gain 6
                                                            //      001=gain 1
                                                            //      010=gain 2
                                                            //      011=gain 3
                                                            //      100=gain 4
                                                            //      101=gain 8
                                                            //      110=gain 12

                                                            // D2:0 =00 external signal
                                                            //      =001input shorted
                                                            //      =101 test signal

cSpiTxBuffer[i++] =0x00;
//cSpiTxBuffer[i++] = 0x20;  // 0x10;  //external signal channel
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;////VOLTAGE_MEASUREMENT; //NOISE_MEASUREMENT;  // internal signal
i=SendtoAFE2 (i,20);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


#if Jorjin
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x65;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;


#else

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;

#endif





cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH4SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;;//NOISE_MEASUREMENT; //noise  masurement
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] =AFE_OPCODE_WREG | AFE_CH5SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x10;//NOISE_MEASUREMENT|CH_PDWN;;//|CH_PDWN;//NOISE_MEASUREMENT;//internal test signal CH_GAIN|CH2_P_RLD

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH6SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT; EXTERNAL_SIG//NOISE_MEASUREMENT;//internal test signal
                                                // D6:4 = GAIN..
                                                // 000=6,001=1,010=2,011=3,100=4,101=8,110=12
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH7SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH8SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x10;// NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH;;//EXTERNAL_SIG; //internal test signal                                  //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);


i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////
#if Jorjin


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
#else
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//////////////////////////////tAKEN FROM EVAL BAORD//////////////////////////////////////////////////////

//disable resipration ckt
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG4; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//disable resipration ckt
//LOFF_SENSP
//LOFF_SENSN
//LOFF_FLIP
//LOFF_STATP
//LOFF_STATN
//GPIO
//PACE
//RESP

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_FLIP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


/////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_GPIO; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



//////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_RESP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

#endif
//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x0b; //wct1 connected to ra lead1 channel1=0x09  //chanel2=0x0b                     //
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT2;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0xd4;// LA ann LL

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

//cSpiTxBuffer[i] = 0xFF;
//i=SendtoAFE2 (i,0);                               //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
ResetScanBuffer();

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);


cSpiTxBuffer[0] = AFE_OPCODE_STOP;
i=SendtoAFE2 (1,0);

cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2 (1,0);
DelayuS(10000);
/*
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
*/



ResetScanBuffer();
cSpiTxferCmplete=0;

/*
DelayuS(10);
DelayuS(10);
DelayuS(10);
*/

Error_init(&eb);
Swi_Params_init(&swiParams);


hswiDRDY = Swi_create((ti_sysbios_knl_Swi_FuncPtr)swiDRDYFunction, &swiParams,&eb);     // this sofwtare interrupt will be generated by DRDY HWI

if (hswiDRDY == NULL) {
    return AFE_INIT_ERROR;                          // eror occured while creating drdry function

}

//---------------------------------------------------------

AFEstart();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
SendtoAFE (handle_spi,1,0);                                 //restart continuous read

AFEChipUnSelect();
DelayuS(10);
AFEChipSelect();

SPI_close(handle_spi);
SPI_init();
SPI_Params_init(&params_spi);
params_spi.frameFormat =SPI_POL0_PHA1;             // this is Motorola format
params_spi.bitRate  =4000000;  //4mhz
params_spi.transferTimeout=1000;
params_spi.mode        = SPI_MASTER;
params_spi.transferMode=SPI_MODE_CALLBACK;//SPI_MODE_BLOCKING
params_spi.transferCallbackFxn=SPICallbackFunction;
handle_spi = SPI_open(SPI0, &params_spi);               // initialise SPI

DelayuS(10);
cInterprocessSem=0;



//hCpuPins= PIN_open(&pinState,& Pin);

cPenidingEvtf=PIN_registerIntCb(hCpuPins, DRDY_Callback);
////i=cPenidingEvtf;
//
//
//
PIN_setConfig(hCpuPins, PIN_BM_IRQ, 4   | PIN_IRQ_NEGEDGE);
PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
ResetScanBuffer();

return AFE_INIT_SUCCESS;


}




/**----------------------------------------------------------------
int AFEinit4_Squarewave()DES: dESC
input: none
returns:AFE_INIT_ERROR(0) if Init not succesful, AFE_INIT_SUCCESS(1) if succesful
----------------------------------------------------------------*/
int AFEinit4_Squarewave(uint16_t iSamplerate)
{
uint16_t   iSamplerateChange;
int i=0,j=0,ch=0;

iSamplerateChange=iSamplerate;

// clear the content
for(j=0;j<10;j++){
    for(i=0;i<27;i++){
        cbleTxBfr_1[j][i]=0x00;
        cbleTxBfr_2[j][i]=0x00;
    }
}
i=0;
// test rouitne to check the gpio
PIN_init(BoardGpioInitTable1);
hCpuPins= PIN_open(&pinState,BoardGpioInitTable1);
//return 1;



//
// 1
//
////deBUG
//
////PIN_setOutputValue(hCpuPins, AFE_CS,0);//CONFIG_SPI_0 MOSI
////PIN_setOutputValue(hCpuPins,1,0);//CONFIG_SPI_0 SCLK
////PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CS
//
//cPenidingEvtf=PIN_registerIntCb(hCpuPins,DRDY_Callback);
//
//PIN_setConfig(hCpuPins, PIN_BM_IRQ,4  | PIN_IRQ_NEGEDGE);
//PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
//deBUG




SPI_init();

//-------SPI initalisation-----------
SPI_Params_init(&params_spi);                       // initialises parameters to defaults
params_spi.bitRate  = 500000;
params_spi.transferTimeout=100;
params_spi.transferMode=SPI_MODE_BLOCKING;              //SPI_MODE_CALLBACK;
//params.transferCallbackFxn=SPICallbackFunction;
params_spi.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
params_spi.mode        = SPI_MASTER;
handle_spi = SPI_open(0, &params_spi);              // initialise SPI


//handle_spi;
spiHandle=handle_spi;
bDataReadinProgress=0;


if (!spiHandle) {
    iSPIStatus=0;
    return AFE_INIT_ERROR;
} else {
    iSPIStatus=1;                                       // MARK SPI open is succesful
}


HardwareResetAFE();
ResetAFE();
DelayuS(10);
AFEChipSelect();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_STOP;
cSpiTxBuffer[1] = AFE_OPCODE_SDATAC;
cSpiTxBuffer[2] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[3] = 0;                             // we need to read one ID register
cSpiTxBuffer[4] = 0xFF;
i=SendtoAFE2 (4,1);                             //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
if (cSpiRxBuffer1[0]!=AFE_DEFINED_ID){
    return AFE_INIT_ERROR;
}

DelayuS(10);

//-------------------------------------------------------------------------------------------------------------------------

//--------------------configuration1 AS PER THE DATASHEET ------------------------


cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2(1,0);

cSpiTxBuffer[0] = AFE_OPCODE_RESET;
i=SendtoAFE2(1,0);
DelayuS(10); //internal voltage startup time


if(iSamplerateChange==250){
    i=0;
    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] =0x4C;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    //cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
    DelayuS(15000); //internal voltage startup time=150ms

    cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
    cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
    cSpiTxBuffer[2] = 0xFF;
    i=SendtoAFE2 (2,40);

    i=0;

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x86;
}else{

    i=0;
    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG3;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] =0xCC;//0xC6;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    //cSpiTxBuffer[i++] =0xCC;   //fed externallyAVSS+AVDD/2 externally; 0x04  RLD AmplifierEnabled

    i=SendtoAFE2 (i,40);                             //Read ID byte. It should be 0x92
    DelayuS(15000); //internal voltage startup time=150ms

    cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
    cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
    cSpiTxBuffer[2] = 0xFF;
    i=SendtoAFE2 (2,40);

    i=0;

}

if(iSamplerateChange==500){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x86;
}// high frequency mode


if(iSamplerateChange==1000){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x85;
}// high frequency mode

if(iSamplerateChange==2000){

    cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG1;
    cSpiTxBuffer[i++] = 0x00;
    cSpiTxBuffer[i++] = 0x84;
}// high frequency mode

//cSpiTxBuffer[i++] = 0x04;// high frequency mode

//cSpiTxBuffer[i++] = 0x85;
                                    //D2:0 data output rate
                                        //06=250 SPS,04=1KSPS,03=2kSPS
                                        // D8=0 LP mode, 1 HR mode
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG2;
cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] =0x00;                  //NO_INTERNAL_TEST_SIGNAL;
cSpiTxBuffer[i++] =0x10;       //0xx000             //INTERNAL_TEST_SIGNAL ON;                       //D4=INT_TEST=1 generates internal etst signal, =0 no test signal
                                        //D1:0=00 or 01 decides freq of test signal
                                        // D2=amplitude 0 lower, 1 higher amplitude

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;
                                        //RLDREF signalis internal
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH1SET;           //   D6:4 PGA gain .
                                                            //      000=gain 6
                                                            //      001=gain 1
                                                            //      010=gain 2
                                                            //      011=gain 3
                                                            //      100=gain 4
                                                            //      101=gain 8
                                                            //      110=gain 12

                                                            // D2:0 =00 external signal
                                                            //      =001input shorted
                                                            //      =101 test signal

cSpiTxBuffer[i++] =0x00;
//cSpiTxBuffer[i++] = 0x20;  // 0x10;  //external signal channel
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;////VOLTAGE_MEASUREMENT; //NOISE_MEASUREMENT;  // internal signal
i=SendtoAFE2 (i,20);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


#if Jorjin
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x65;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;


#else

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH2SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;//CH_GAIN|CH2_P_RLD; //Channel 2 RLD Enabled // EXTERNAL_SIG
                        /////NOISE_MEASUREMENT|CH_PDWN;////;//INTERNAL_TEST_SIGNAL_CH; //NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT;//NOISE_MEASUREMENT;
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH3SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x15;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;

#endif





cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH4SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;;//NOISE_MEASUREMENT; //noise  masurement
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] =AFE_OPCODE_WREG | AFE_CH5SET;
cSpiTxBuffer[i++] =0x00;
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x10;//NOISE_MEASUREMENT|CH_PDWN;;//|CH_PDWN;//NOISE_MEASUREMENT;//internal test signal CH_GAIN|CH2_P_RLD

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH6SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;//VOLTAGE_MEASUREMENT; EXTERNAL_SIG//NOISE_MEASUREMENT;//internal test signal
                                                // D6:4 = GAIN..
                                                // 000=6,001=1,010=2,011=3,100=4,101=8,110=12
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH7SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x15;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CH8SET;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x15;// NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH;;//EXTERNAL_SIG; //internal test signal                                  //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);


i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////
#if Jorjin


cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x04;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
#else
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);
//
//
i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;
//
//
//
//

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x02;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//////////////////////////////tAKEN FROM EVAL BAORD//////////////////////////////////////////////////////

//disable resipration ckt
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG4; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


//disable resipration ckt
//LOFF_SENSP
//LOFF_SENSN
//LOFF_FLIP
//LOFF_STATP
//LOFF_STATN
//GPIO
//PACE
//RESP

cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_SENSN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_LOFF_FLIP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;




cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_STATN; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


/////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_LOFF_GPIO; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



//////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_RESP; //sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x00;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;

#endif
//////////////////////////////////////////////RLD SENSE/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT1;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x00; //wct1 connected to ra lead1 channel1=0x09  //chanel2=0x0b                     //
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;



cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT2;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x00;// LA ann LL

i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);

//cSpiTxBuffer[i] = 0xFF;
//i=SendtoAFE2 (i,0);                               //Read ID byte. It should be 0x92
if (i==AFE_TRANSFER_ERR){
    return AFE_INIT_ERROR;
}
ResetScanBuffer();

cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);


cSpiTxBuffer[0] = AFE_OPCODE_STOP;
i=SendtoAFE2 (1,0);

cSpiTxBuffer[0] = AFE_OPCODE_SDATAC;
i=SendtoAFE2 (1,0);
DelayuS(10000);
/*
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
DelayuS(10000);
*/



ResetScanBuffer();
cSpiTxferCmplete=0;

/*
DelayuS(10);
DelayuS(10);
DelayuS(10);
*/

Error_init(&eb);
Swi_Params_init(&swiParams);


hswiDRDY = Swi_create((ti_sysbios_knl_Swi_FuncPtr)swiDRDYFunction, &swiParams,&eb);     // this sofwtare interrupt will be generated by DRDY HWI

if (hswiDRDY == NULL) {
    return AFE_INIT_ERROR;                          // eror occured while creating drdry function

}

//---------------------------------------------------------

AFEstart();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
SendtoAFE (handle_spi,1,0);                                 //restart continuous read

AFEChipUnSelect();
DelayuS(10);
AFEChipSelect();

SPI_close(handle_spi);
SPI_init();
SPI_Params_init(&params_spi);
//params_spi.frameFormat =SPI_POL0_PHA1;             // this is Motorola format

params_spi.frameFormat=SPI_POL0_PHA1;
params_spi.bitRate  =10000000;  //4mhz
params_spi.transferTimeout=1000;
params_spi.mode        = SPI_MASTER;
params_spi.transferMode=SPI_MODE_CALLBACK;//SPI_MODE_BLOCKING
params_spi.transferCallbackFxn=SPICallbackFunction;
handle_spi = SPI_open(SPI0, &params_spi);               // initialise SPI

DelayuS(10);
cInterprocessSem=0;



//hCpuPins= PIN_open(&pinState,& Pin);

cPenidingEvtf=PIN_registerIntCb(hCpuPins, DRDY_Callback);
////i=cPenidingEvtf;
//
//
//
PIN_setConfig(hCpuPins, PIN_BM_IRQ, 4   | PIN_IRQ_NEGEDGE);
PIN_setInterrupt(hCpuPins, 4|PIN_IRQ_NEGEDGE);
ResetScanBuffer();

return AFE_INIT_SUCCESS;


}



/****************************************************************************************8
Sydantek_Afeinit()




****************************************************************************************/
void Sydantek_Afeinit()
{

uint8_t status,statusbreakpoint;

//status = sem_init(&masterSem, 0, 0);

   while(1){
    status=AFEinit4();

    Sample_rateCopy=Sample_rate =Sample_rate=params.limb_hz;



    if(status!=0){
        UART_init();
        SydantekAdc_Init();

        /* Create a UART with data processing off. */
        UART_Params_init(&uartParams);
        uartParams.baudRate = 500000;

        uart = UART_open(CONFIG_UART_0, &uartParams);

        if (uart == NULL) {
            /* UART_open() failed */
            while (1);
        }

        break;
    }else{
        statusbreakpoint=0;
    }
   }

}







//uint8_t Sydantek_ProcessTimerEvent(UInt32 Sytemtime)
//{
//    static uint32_t attempts = 0;
//    static uint32_t successes = 0;
//    static uint16_t imiliTime=0x01; // IT WILL BE ONE
//    static uint16_t  imiliTimeCopy=0x00;
//    static uint8_t clocal[247],cErrorframe[247],checksum;
//    uint8_t cNotiTemp[10][27];
//    uint8_t ch=0,ch1=0;
//    int i1=0,i2=0,i3=0,i4=0,ix,jx,i,in,io,j,k;
//    attHandleValueNoti_t notification;
//    bStatus_t status;
//    imiliTime  =Sytemtime;
//
//
//
//
////    Sample_rate =params.limb_hz;
////    if(Sample_rate!=Sample_rateCopy){
////
////         Sample_rateCopy=Sample_rate;
////         SPI_close(handle_spi);
////         AFEinit4_Changefrequecny(Sample_rate);
////         iReadbatCount=0;// resetthe battery countinoder not get the batlow and chnge in smapling rate
////    }
////    iReadbatCount++;
////    if( iReadbatCount>=READ_BAT_SEC){
////         params.iBatvalue=SydantekReadadc();
////         iReadbatCount=0;
////         if(params.iBatvalue<=BAT_LOW_VAL  && cBatlevelchange==BAT_HIGH ){
////             SPI_close(handle_spi);
////             AFEinit4_Squarewave(Sample_rate);
////             cBatlevelchange=BAT_LOW;
////
////        }else{
////             if(params.iBatvalue>BAT_LOW_VAL && cBatlevelchange==BAT_LOW){
////                 SPI_close(handle_spi);
////                 AFEinit4_Changefrequecny(Sample_rate);
////                 cBatlevelchange=BAT_HIGH;
////             }
////          }
////         }
////
////
//
//
//
//    if(imiliTime <= imiliTimeCopy){
//            imiliTime=imiliTimeCopy+1;// sync with rtc is not happend
//     }
//
//    //return 1;
//    #if  UARTnBLEDump
//
//
//         if(cbfrfilled<=0) return 0; // buffer not filled
//         ch1=cbfrfilled;
//         cbfrfilled=0;
//         if(ch1==1){ //buffer filled with 1
//             for(j=0;j<10;j++){
//                 for(i=0;i<27;i++){ //leave the status
//                     cNotiTemp[j][i]=cbleTxBfr_1[j][i];
//                 }
//             }
//
//         }
//
//         if(ch1==2){//buffer filled with 2
//             for(j=0;j<10;j++){
//                 for(i=0;i<27;i++){ //leave the status
//                     cNotiTemp[j][i]=cbleTxBfr_2[j][i];
//                 }
//             }
//
//         }
//         gattAttribute_t* attribute = GATTServApp_FindAttr(
//                    attributes, GATT_NUM_ATTRS(attributes), notify);
//         notification.handle = attribute->handle;
//
//         for ( i = 0; i < linkDBNumConns; ++i) {
//            gattCharCfg_t *item = notify_config + i;
//            if (item->connHandle == LINKDB_CONNHANDLE_INVALID) continue;
//            if (item->value == GATT_CFG_NO_OPERATION) continue;
//            notification.pValue = (uint8_t*) GATT_bm_alloc(
//                        item->connHandle, ATT_HANDLE_VALUE_NOTI, PAYLOAD_BYTES,&notification.len);
//            if (!notification.pValue) break;
//
//                uint8_t* pointer = notification.pValue;
//
//                uint8_t* pRef = notification.pValue;
//
////                uint8_t* Upointer=clocal;
////                uint8_t* Urpointer=clocal;
////
////                uint8_t* EUpointer=cErrorframe;
////                uint8_t* EUrpointer=cErrorframe;
//
//                *(uint8_t*) pointer =imiliTime & 0xff; //Sytemtime;
//                 pointer+=1;
//
//               *(uint8_t*) pointer =(imiliTime & 0xff00)>>8; //Sytemtime;
//                 pointer+=1;
//
//
//                *(uint16_t*) pointer=0x00; //Sytemtime; //Sytemtime;
//                 pointer+=2;
////                 checksum=0;
////
////
////                 *(uint8_t*) Upointer =SYN;//0xaa & 0xff; //Sytemtime;
////                 checksum+=SYN;
////                 Upointer+=1;
////
////                *(uint8_t*) Upointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
////                checksum+=ID;
////                 Upointer+=1;
////
////
////                *(uint8_t*)Upointer=imiliTime & 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
////                checksum+=0x0;
////                 Upointer+=1;
////
////
////               *(uint8_t*)Upointer=(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
////                 checksum+=0x0;
////                 Upointer+=1;
////                 //////////////////////////////////////////
////
////                 *(uint8_t*) EUpointer =SYN;//0xaa & 0xff; //Sytemtime;
////                 //checksum=SYN;
////                 EUpointer+=1;
////
////                *(uint8_t*) EUpointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
////                //checksum=ID;
////                 EUpointer+=1;
////
////
////                *(uint8_t*)EUpointer=imiliTime & 0xff;; //& 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
////                //checksum+=0x0;
////                 EUpointer+=1;
////
////
////               *(uint8_t*)EUpointer=(imiliTime & 0xff00)>>8;//(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
////                 //checksum+=0x0;
////                 EUpointer+=1;
////
//
//
//
//
//                    //
// //
//                 pRef=pointer;
//                 for(i4=0;i4<240;i4++){
//                     *(uint8_t*)pointer++=0x00;
//                 }
//
//                 pointer=pRef;
//
//
////                 Urpointer=Upointer;
////                 for(i4=0;i4<240;i4++){
////                     *(uint8_t*)Upointer++=0X00;
////                 }
////
////                 Upointer=Urpointer;
////                 //
////
////                 EUrpointer=EUpointer;
////                  for(i4=0;i4<240;i4++){
////                      *(uint8_t*)EUpointer++=0X00;
////                  }
////
////                  EUpointer=EUrpointer;
//                 i3=1;
//
//                 xl=0;
//                     for (i2=0;i2<=9;i2++){
//                               for (jx=1;jx<9;jx++){
//                                   if(jx==1)ch=LEAD_1;
//                                   if(jx==2)ch=LEAD_2;
//                                   if(jx==3)ch=V1;
//                                   if(jx==4)ch=V2;
//                                   if(jx==5)ch=V3;
//                                   if(jx==6)ch=V4;
//                                   if(jx==7)ch=V5;
//                                   if(jx==8)ch=V6;
//
//
//                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+2];
//                                       *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+1];
//                                       *(uint8_t*)pointer++=cNotiTemp[i2][(ch)];
//
//
////                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+2];
////                                       checksum+=cNotiTemp[i2][(ch)+2];
////                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+1];
////                                       checksum+=cNotiTemp[i2][(ch)+1];
////                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)];
////                                       checksum+=cNotiTemp[i2][(ch)+1];
//                        }
//
//                     }
//
////                     *(uint8_t*)Upointer++=ETX;
////                      checksum+=ETX;
////
////                      *(uint8_t*)Upointer++=checksum;
////                      *(uint8_t*)Upointer++=EOT;
//
//
//                 ++attempts;
//                    if(i3==1){
//    //status = GATT_Indication(item->connHandle,(attHandleValueInd_t *) &notification,FALSE,0);
//            status = GATT_Notification(item->connHandle, &notification, FALSE);
//                        if (status != SUCCESS) {
////                            for (i2=0;i2<=9;i2++){
////                                for (jx=0;jx<8;jx++){
////                                    if(jx==3){
////                                        *(uint8_t*)EUpointer++=status & 0xff;//cSpiRxBuffer2[(ch)+2];
////                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)+1];
////                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)];
////
////                                    }else{
////
////                                        *(uint8_t*)EUpointer++=clocal[jx];//cSpiRxBuffer2[(ch)+2];
////                                        *(uint8_t*)EUpointer++=clocal[jx+1];//cSpiRxBuffer2[(ch)+1];
////                                        *(uint8_t*)EUpointer++=clocal[jx+2];//cSpiRxBuffer2[(ch)];
////
////                                    }
////                                }
////                            }
////
////                            *(uint8_t*)EUpointer++=ETX;
////                            *(uint8_t*)EUpointer++=checksum;
////                            *(uint8_t*)EUpointer++=EOT;
//                             //UART_write(uart, cErrorframe, sizeof(cErrorframe));
//                            //Log_info1("Notification: status=%d", status);
//                            GATT_bm_free((gattMsg_t *) &notification, ATT_HANDLE_VALUE_NOTI);
//                            imiliTimeCopy=imiliTime; //real data loss
//
//                        } else {
//
//                            imiliTimeCopy=imiliTime;
//                             //UART_write(uart, clocal, sizeof(clocal));
//                            //iDrdyCount=0;
//                        }
//                   }
//                }
//            //}
//            return 0;
//
//            #endif
//
//            #if UARTDUMP
//
//                    ix=ScanBuffer.iCount;
//                    in =ScanBuffer.iIptr;
//                    io =ScanBuffer.iOptr;
//                    if (ix<=9) return 0;
//                    clocal[0]=0x1;
//                    clocal[1]=0x2;
//                    clocal[2]=0x3;
//                    i=1;
//
//
//                        for (i2=0;i2<=9;i2++){
//
//                           i1=GetDataFromScanBuffer();
//                               if (i1!=0){
//                                   for (jx=1;jx<9;jx++){
//                                       if(jx==1)ch=LEAD_1;
//                                       if(jx==2)ch=LEAD_2;
//                                       if(jx==3)ch=V1;
//                                       if(jx==4)ch=V2;
//                                       if(jx==5)ch=V3;
//                                       if(jx==6)ch=V4;
//                                       if(jx==7)ch=V5;
//                                       if(jx==8)ch=V6;
//
//
//                                       clocal[(i*3)+2]=0xff;//cSpiRxBuffer2[(ch)+2];
//                                       clocal[(i*3)+1]=0x55;//cSpiRxBuffer2[(ch)+1];
//                                       clocal[(i*3)]=0xaa;//cSpiRxBuffer2[(ch)];
//
//
//
//                                }
//                               }
//
//                            i++;
//                        }
//                        UART_write(uart, &clocal, sizeof(clocal));
//                 return 0;
//
//            #endif
//
//}
//





uint8_t Sydantek_ProcessTimerEvent(UInt32 Sytemtime)
{
    static uint32_t attempts = 0;
    static uint32_t successes = 0;
    static uint16_t imiliTime=0x01; // IT WILL BE ONE
    static uint16_t  imiliTimeCopy=0x00;
    static uint8_t clocal[247],cErrorframe[247],checksum;
    uint8_t cNotiTemp[10][27];
    uint8_t ch=0,ch1=0;
    int i1=0,i2=0,i3=0,i4=0,ix,jx,i,in,io,j,k;
    attHandleValueNoti_t notification;
    bStatus_t status;
    imiliTime  =Sytemtime;




//    Sample_rate =params.limb_hz;
//    if(Sample_rate!=Sample_rateCopy){
//
//         Sample_rateCopy=Sample_rate;
//         SPI_close(handle_spi);
//         AFEinit4_Changefrequecny(Sample_rate);
//         iReadbatCount=0;// resetthe battery countinoder not get the batlow and chnge in smapling rate
//    }
//    iReadbatCount++;
//    if( iReadbatCount>=READ_BAT_SEC){
//         params.iBatvalue=SydantekReadadc();
//         iReadbatCount=0;
//         if(params.iBatvalue<=BAT_LOW_VAL  && cBatlevelchange==BAT_HIGH ){
//             SPI_close(handle_spi);
//             AFEinit4_Squarewave(Sample_rate);
//             cBatlevelchange=BAT_LOW;
//
//        }else{
//             if(params.iBatvalue>BAT_LOW_VAL && cBatlevelchange==BAT_LOW){
//                 SPI_close(handle_spi);
//                 AFEinit4_Changefrequecny(Sample_rate);
//                 cBatlevelchange=BAT_HIGH;
//             }
//          }
//         }
//
//



    if(imiliTime <= imiliTimeCopy){
            imiliTime=imiliTimeCopy+1;// sync with rtc is not happend
     }
    #if  UARTnBLEDump


         if(cbfrfilled<=0) return 0; // buffer not filled
         ch1=cbfrfilled;
         cbfrfilled=0;
         if(ch1==1){ //buffer filled with 1
             for(j=0;j<10;j++){
                 for(i=0;i<27;i++){ //leave the status
                     cNotiTemp[j][i]=cbleTxBfr_1[j][i];
                 }
             }

         }

         if(ch1==2){//buffer filled with 2
             for(j=0;j<10;j++){
                 for(i=0;i<27;i++){ //leave the status
                     cNotiTemp[j][i]=cbleTxBfr_2[j][i];
                 }
             }

         }
         gattAttribute_t* attribute = GATTServApp_FindAttr(
                    attributes, GATT_NUM_ATTRS(attributes), notify);
         notification.handle = attribute->handle;

         for ( i = 0; i < linkDBNumConns; ++i) {
            gattCharCfg_t *item = notify_config + i;
            if (item->connHandle == LINKDB_CONNHANDLE_INVALID) continue;
            if (item->value == GATT_CFG_NO_OPERATION) continue;
            notification.pValue = (uint8_t*) GATT_bm_alloc(
                        item->connHandle, ATT_HANDLE_VALUE_NOTI, PAYLOAD_BYTES,&notification.len);
            if (!notification.pValue) break;

                uint8_t* pointer = notification.pValue;

                uint8_t* pRef = notification.pValue;

                uint8_t* Upointer=clocal;
                uint8_t* Urpointer=clocal;

                uint8_t* EUpointer=cErrorframe;
                uint8_t* EUrpointer=cErrorframe;

                *(uint8_t*) pointer =imiliTime & 0xff; //Sytemtime;
                 pointer+=1;

               *(uint8_t*) pointer =(imiliTime & 0xff00)>>8; //Sytemtime;
                 pointer+=1;


                *(uint16_t*) pointer=0x00; //Sytemtime; //Sytemtime;
                 pointer+=2;
                 checksum=0;


                 *(uint8_t*) Upointer =SYN;//0xaa & 0xff; //Sytemtime;
                 checksum+=SYN;
                 Upointer+=1;

                *(uint8_t*) Upointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
                checksum+=ID;
                 Upointer+=1;


                *(uint8_t*)Upointer=imiliTime & 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                checksum+=0x0;
                 Upointer+=1;


               *(uint8_t*)Upointer=(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                 checksum+=0x0;
                 Upointer+=1;
                 //////////////////////////////////////////

                 *(uint8_t*) EUpointer =SYN;//0xaa & 0xff; //Sytemtime;
                 //checksum=SYN;
                 EUpointer+=1;

                *(uint8_t*) EUpointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
                //checksum=ID;
                 EUpointer+=1;


                *(uint8_t*)EUpointer=imiliTime & 0xff;; //& 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                //checksum+=0x0;
                 EUpointer+=1;


               *(uint8_t*)EUpointer=(imiliTime & 0xff00)>>8;//(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                 //checksum+=0x0;
                 EUpointer+=1;





                    //
 //
                 pRef=pointer;
                 for(i4=0;i4<240;i4++){
                     *(uint8_t*)pointer++=0x00;
                 }

                 pointer=pRef;


                 Urpointer=Upointer;
                 for(i4=0;i4<240;i4++){
                     *(uint8_t*)Upointer++=0X00;
                 }

                 Upointer=Urpointer;
                 //

                 EUrpointer=EUpointer;
                  for(i4=0;i4<240;i4++){
                      *(uint8_t*)EUpointer++=0X00;
                  }

                  EUpointer=EUrpointer;
                 i3=1;

                 xl=0;
                     for (i2=0;i2<=9;i2++){
                               for (jx=1;jx<9;jx++){
                                   if(jx==1)ch=LEAD_1;
                                   if(jx==2)ch=LEAD_2;
                                   if(jx==3)ch=V1;
                                   if(jx==4)ch=V2;
                                   if(jx==5)ch=V3;
                                   if(jx==6)ch=V4;
                                   if(jx==7)ch=V5;
                                   if(jx==8)ch=V6;

/*
                                   if(jx==3){
                                      *(uint8_t*)pointer++=ch1 & 0xff;//cSpiRxBuffer2[(ch)+2];
                                       *(uint8_t*)pointer++=0x00;//(imiliTime & 0xff00)>>8;//cSpiRxBuffer2[(ch)+1];
                                       *(uint8_t*)pointer++=0x0;//cSpiRxBuffer2[(ch)];

                                   }else{

                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+2];
                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+1];
                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)];
                                   }
*/

                                   if(jx==3){
                                       *(uint8_t*)pointer++=imiliTime & 0xff;//cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)pointer++=(imiliTime & 0xff00)>>8;//cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)pointer++=0x0;//cNotiTemp[i2][(ch)];


                                       *(uint8_t*)Upointer++=imiliTime & 0xff;///cNotiTemp[i2][(ch)+2];
                                       checksum+=imiliTime & 0xff;//cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)Upointer++=(imiliTime & 0xff00)>>8;//cNotiTemp[i2][(ch)+1];
                                       checksum+=(imiliTime & 0xff00)>>8;//cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)Upointer++=0x0;//cNotiTemp[i2][(ch)];
                                       checksum+=0x0;//cNotiTemp[i2][(ch)+1];

                                   }else{
                                       *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)pointer++=cNotiTemp[i2][(ch)];


                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+2];
                                       checksum+=cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+1];
                                       checksum+=cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)];
                                       checksum+=cNotiTemp[i2][(ch)+1];
                                   }
                        }

                     }

                     *(uint8_t*)Upointer++=ETX;
                      checksum+=ETX;

                      *(uint8_t*)Upointer++=checksum;
                      *(uint8_t*)Upointer++=EOT;


                 ++attempts;
                    if(i3==1){
    //status = GATT_Indication(item->connHandle,(attHandleValueInd_t *) &notification,FALSE,0);
            status = GATT_Notification(item->connHandle, &notification, FALSE);
                        if (status != SUCCESS) {
                            for (i2=0;i2<=9;i2++){
                                for (jx=0;jx<8;jx++){
                                    if(jx==3){
                                        *(uint8_t*)EUpointer++=status & 0xff;//cSpiRxBuffer2[(ch)+2];
                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)+1];
                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)];

                                    }else{

                                        *(uint8_t*)EUpointer++=clocal[jx];//cSpiRxBuffer2[(ch)+2];
                                        *(uint8_t*)EUpointer++=clocal[jx+1];//cSpiRxBuffer2[(ch)+1];
                                        *(uint8_t*)EUpointer++=clocal[jx+2];//cSpiRxBuffer2[(ch)];

                                    }
                                }
                            }

                            *(uint8_t*)EUpointer++=ETX;
                            *(uint8_t*)EUpointer++=checksum;
                            *(uint8_t*)EUpointer++=EOT;
                             //UART_write(uart, cErrorframe, sizeof(cErrorframe));
                            //Log_info1("Notification: status=%d", status);
                            GATT_bm_free((gattMsg_t *) &notification, ATT_HANDLE_VALUE_NOTI);
                            imiliTimeCopy=imiliTime; //real data loss

                        } else {

                            imiliTimeCopy=imiliTime;
                             //UART_write(uart, clocal, sizeof(clocal));
                            //iDrdyCount=0;
                        }
                   }
                }
            //}
            return 0;

            #endif

            #if UARTDUMP

                    ix=ScanBuffer.iCount;
                    in =ScanBuffer.iIptr;
                    io =ScanBuffer.iOptr;
                    if (ix<=9) return 0;
                    clocal[0]=0x1;
                    clocal[1]=0x2;
                    clocal[2]=0x3;
                    i=1;


                        for (i2=0;i2<=9;i2++){

                           i1=GetDataFromScanBuffer();
                               if (i1!=0){
                                   for (jx=1;jx<9;jx++){
                                       if(jx==1)ch=LEAD_1;
                                       if(jx==2)ch=LEAD_2;
                                       if(jx==3)ch=V1;
                                       if(jx==4)ch=V2;
                                       if(jx==5)ch=V3;
                                       if(jx==6)ch=V4;
                                       if(jx==7)ch=V5;
                                       if(jx==8)ch=V6;


                                       clocal[(i*3)+2]=0xff;//cSpiRxBuffer2[(ch)+2];
                                       clocal[(i*3)+1]=0x55;//cSpiRxBuffer2[(ch)+1];
                                       clocal[(i*3)]=0xaa;//cSpiRxBuffer2[(ch)];



                                }
                               }

                            i++;
                        }
                        UART_write(uart, &clocal, sizeof(clocal));
                 return 0;

            #endif

}




uint8_t Sydantek_ProcessTimerEvent_uarttrnsmit(UInt32 Sytemtime)
{
    static uint32_t attempts = 0;
    static uint32_t successes = 0;
    static uint16_t imiliTime=0x01; // IT WILL BE ONE
    static uint16_t  imiliTimeCopy=0x00;
    static uint8_t clocal[247],cErrorframe[247],checksum;
    uint8_t cNotiTemp[10][27];
    uint8_t ch=0,ch1=0;
    int i1=0,i2=0,i3=0,i4=0,ix,jx,i,in,io,j,k;
    attHandleValueNoti_t notification;
    bStatus_t status;
    imiliTime  =Sytemtime;




//    Sample_rate =params.limb_hz;
//    if(Sample_rate!=Sample_rateCopy){
//
//         Sample_rateCopy=Sample_rate;
//         SPI_close(handle_spi);
//         AFEinit4_Changefrequecny(Sample_rate);
//         iReadbatCount=0;// resetthe battery countinoder not get the batlow and chnge in smapling rate
//    }
//    iReadbatCount++;
//    if( iReadbatCount>=READ_BAT_SEC){
//         params.iBatvalue=SydantekReadadc();
//         iReadbatCount=0;
//         if(params.iBatvalue<=BAT_LOW_VAL  && cBatlevelchange==BAT_HIGH ){
//             SPI_close(handle_spi);
//             AFEinit4_Squarewave(Sample_rate);
//             cBatlevelchange=BAT_LOW;
//
//        }else{
//             if(params.iBatvalue>BAT_LOW_VAL && cBatlevelchange==BAT_LOW){
//                 SPI_close(handle_spi);
//                 AFEinit4_Changefrequecny(Sample_rate);
//                 cBatlevelchange=BAT_HIGH;
//             }
//          }
//         }
//
//



    if(imiliTime <= imiliTimeCopy){
            imiliTime=imiliTimeCopy+1;// sync with rtc is not happend
     }
    #if  UARTnBLEDump


         if(cbfrfilled<=0) return 0; // buffer not filled
         ch1=cbfrfilled;
         cbfrfilled=0;
         if(ch1==1){ //buffer filled with 1
             for(j=0;j<10;j++){
                 for(i=0;i<27;i++){ //leave the status
                     cNotiTemp[j][i]=cbleTxBfr_1[j][i];
                 }
             }

         }

         if(ch1==2){//buffer filled with 2
             for(j=0;j<10;j++){
                 for(i=0;i<27;i++){ //leave the status
                     cNotiTemp[j][i]=cbleTxBfr_2[j][i];
                 }
             }

         }

                uint8_t* Upointer=clocal;
                uint8_t* Urpointer=clocal;



                 *(uint8_t*) Upointer =SYN;//0xaa & 0xff; //Sytemtime;
                 checksum+=SYN;
                 Upointer+=1;

                *(uint8_t*) Upointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
                checksum+=ID;
                 Upointer+=1;


                *(uint8_t*)Upointer=imiliTime & 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                checksum+=0x0;
                 Upointer+=1;


               *(uint8_t*)Upointer=(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
                 checksum+=0x0;
                 Upointer+=1;
                 //////////////////////////////////////////

                    //
 //

                 Urpointer=Upointer;
                 for(i4=0;i4<240;i4++){
                     *(uint8_t*)Upointer++=0X00;
                 }

                 Upointer=Urpointer;
                 //

                 i3=1;

                 xl=0;
                     for (i2=0;i2<=9;i2++){
                               for (jx=1;jx<9;jx++){
                                   if(jx==1)ch=LEAD_1;
                                   if(jx==2)ch=LEAD_2;
                                   if(jx==3)ch=V1;
                                   if(jx==4)ch=V2;
                                   if(jx==5)ch=V3;
                                   if(jx==6)ch=V4;
                                   if(jx==7)ch=V5;
                                   if(jx==8)ch=V6;


                                   if(jx==3){


                                       *(uint8_t*)Upointer++=imiliTime & 0xff;///cNotiTemp[i2][(ch)+2];
                                       checksum+=imiliTime & 0xff;//cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)Upointer++=(imiliTime & 0xff00)>>8;//cNotiTemp[i2][(ch)+1];
                                       checksum+=(imiliTime & 0xff00)>>8;//cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)Upointer++=0x0;//cNotiTemp[i2][(ch)];
                                       checksum+=0x0;//cNotiTemp[i2][(ch)+1];

                                   }else{

                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+2];
                                       checksum+=cNotiTemp[i2][(ch)+2];
                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+1];
                                       checksum+=cNotiTemp[i2][(ch)+1];
                                       *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)];
                                       checksum+=cNotiTemp[i2][(ch)+1];
                                   }
                        }

                     }

                     *(uint8_t*)Upointer++=ETX;
                      checksum+=ETX;

                      *(uint8_t*)Upointer++=checksum;
                      *(uint8_t*)Upointer++=EOT;


                 ++attempts;
                    if(i3==1){

                            imiliTimeCopy=imiliTime;
                             UART_write(uart, clocal, sizeof(clocal));
                            //iDrdyCount=0;
                        }
            //}
            return 0;

            #endif

            #if UARTDUMP

                    ix=ScanBuffer.iCount;
                    in =ScanBuffer.iIptr;
                    io =ScanBuffer.iOptr;
                    if (ix<=9) return 0;
                    clocal[0]=0x1;
                    clocal[1]=0x2;
                    clocal[2]=0x3;
                    i=1;


                        for (i2=0;i2<=9;i2++){

                           i1=GetDataFromScanBuffer();
                               if (i1!=0){
                                   for (jx=1;jx<9;jx++){
                                       if(jx==1)ch=LEAD_1;
                                       if(jx==2)ch=LEAD_2;
                                       if(jx==3)ch=V1;
                                       if(jx==4)ch=V2;
                                       if(jx==5)ch=V3;
                                       if(jx==6)ch=V4;
                                       if(jx==7)ch=V5;
                                       if(jx==8)ch=V6;


                                       clocal[(i*3)+2]=0xff;//cSpiRxBuffer2[(ch)+2];
                                       clocal[(i*3)+1]=0x55;//cSpiRxBuffer2[(ch)+1];
                                       clocal[(i*3)]=0xaa;//cSpiRxBuffer2[(ch)];



                                }
                               }

                            i++;
                        }
                        UART_write(uart, &clocal, sizeof(clocal));
                 return 0;

            #endif

}











//uint8_t Sydantek_ProcessTimerEvent(UInt32 Sytemtime) {
//    static uint32_t attempts = 0;
//    static uint32_t successes = 0;
//    static uint16_t imiliTime=0x01; // IT WILL BE ONE
//    static uint16_t  imili//    Sample_rate =params.limb_hz;
//    if(Sample_rate!=Sample_rateCopy){
//
//         Sample_rateCopy=Sample_rate;
//         SPI_close(handle_spi);
//         AFEinit4_Changefrequecny(Sample_rate);
//    }
//
//
//    if(imiliTime <= imiliTimeCopy){
//            imiliTime=imiliTimeCopy+1;// sync with rtc is not happend
//     }
//    #if  UARTnBLEDump
//
//
//         if(cbfrfilled<=0) return 0; // buffer not filled
//         ch1=cbfrfilled;
//         cbfrfilled=0;
//         if(ch1==1){ //buffer filled with 1
//             for(j=0;j<10;j++){
//                 for(i=0;i<27;i++){ //leave the status
//                     cNotiTemp[j][i]=cbleTxBfr_1[j][i];
//                 }
//             }
//
//         }
//
//         if(ch1==2){//buffer filled with 2
//             for(j=0;j<10;j++){
//                 for(i=0;i<27;i++){ //leave the status
//                     cNotiTemp[j][i]=cbleTxBfr_2[j][i];
//                 }
//             }
//
//         }
//         gattAttribute_t* attribute = GATTServApp_FindAttr(
//                    attributes, GATT_NUM_ATTRS(attributes), notify);
//         notification.handle = attribute->handle;
//
//         for ( i = 0; i < linkDBNumConns; ++i) {
//            gattCharCfg_t *item = notify_config + i;
//            if (item->connHandle == LINKDB_CONNHANDLE_INVALID) continue;
//            if (item->value == GATT_CFG_NO_OPERATION) continue;
//            notification.pValue = (uint8_t*) GATT_bm_alloc(
//                        item->connHandle, ATT_HANDLE_VALUE_NOTI, PAYLOAD_BYTES,&notification.len);
//            if (!notification.pValue) break;
//
//                uint8_t* pointer = notification.pValue;
//
//                uint8_t* pRef = notification.pValue;
//
//                uint8_t* Upointer=clocal;
//                uint8_t* Urpointer=clocal;
//
//                uint8_t* EUpointer=cErrorframe;
//                uint8_t* EUrpointer=cErrorframe;
//
//                *(uint8_t*) pointer =imiliTime & 0xff; //Sytemtime;
//                 pointer+=1;
//
//               *(uint8_t*) pointer =(imiliTime & 0xff00)>>8; //Sytemtime;
//                 pointer+=1;
//
//
//                *(uint16_t*) pointer=0x00; //Sytemtime; //Sytemtime;
//                 pointer+=2;
//                 checksum=0;
//
//
//                 *(uint8_t*) Upointer =SYN;//0xaa & 0xff; //Sytemtime;
//                 checksum+=SYN;
//                 Upointer+=1;
//
//                *(uint8_t*) Upointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
//                checksum+=ID;
//                 Upointer+=1;
//
//
//                *(uint8_t*)Upointer=imiliTime & 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
//                checksum+=0x0;
//                 Upointer+=1;
//
//
//               *(uint8_t*)Upointer=(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
//                 checksum+=0x0;
//                 Upointer+=1;
//                 //////////////////////////////////////////
//
//                 *(uint8_t*) EUpointer =SYN;//0xaa & 0xff; //Sytemtime;
//                 //checksum=SYN;
//                 EUpointer+=1;
//
//                *(uint8_t*) EUpointer =ID;//(0x55 & 0xff00)>>8; //Sytemtime;
//                //checksum=ID;
//                 EUpointer+=1;
//
//
//                *(uint8_t*)EUpointer=imiliTime & 0xff;; //& 0xff; //Sytemtime ; ;//Sytemtime; //Sytemtime;
//                //checksum+=0x0;
//                 EUpointer+=1;
//
//
//               *(uint8_t*)EUpointer=(imiliTime & 0xff00)>>8;//(imiliTime & 0xff00)>>8;; //Sytemtime ; ;//Sytemtime; //Sytemtime;
//                 //checksum+=0x0;
//                 EUpointer+=1;
//
//
//
//
//
//                    //
// //
//                 pRef=pointer;
//                 for(i4=0;i4<240;i4++){
//                     *(uint8_t*)pointer++=0x00;
//                 }
//
//                 pointer=pRef;
//
//
//                 Urpointer=Upointer;
//                 for(i4=0;i4<240;i4++){
//                     *(uint8_t*)Upointer++=0X00;
//                 }
//
//                 Upointer=Urpointer;
//                 //
//
//                 EUrpointer=EUpointer;
//                  for(i4=0;i4<240;i4++){
//                      *(uint8_t*)EUpointer++=0X00;
//                  }
//
//                  EUpointer=EUrpointer;
//                 i3=1;
//
//                 xl=0;
//                     for (i2=0;i2<=9;i2++){
//                               for (jx=1;jx<9;jx++){
//                                   if(jx==1)ch=LEAD_1;
//                                   if(jx==2)ch=LEAD_2;
//                                   if(jx==3)ch=V1;
//                                   if(jx==4)ch=V2;
//                                   if(jx==5)ch=V3;
//                                   if(jx==6)ch=V4;
//                                   if(jx==7)ch=V5;
//                                   if(jx==8)ch=V6;
//
///*
//                                   if(jx==3){
//                                      *(uint8_t*)pointer++=ch1 & 0xff;//cSpiRxBuffer2[(ch)+2];
//                                       *(uint8_t*)pointer++=0x00;//(imiliTime & 0xff00)>>8;//cSpiRxBuffer2[(ch)+1];
//                                       *(uint8_t*)pointer++=0x0;//cSpiRxBuffer2[(ch)];
//
//                                   }else{
//
//                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+2];
//                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+1];
//                                        *(uint8_t*)pointer++=cNotiTemp[i2][(ch)];
//                                   }
//*/
//
//
//                                   *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+2];
//                                   *(uint8_t*)pointer++=cNotiTemp[i2][(ch)+1];
//                                   *(uint8_t*)pointer++=cNotiTemp[i2][(ch)];
//
//
//                                   *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+2];
//                                   checksum+=cNotiTemp[i2][(ch)+2];
//                                   *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)+1];
//                                   checksum+=cNotiTemp[i2][(ch)+1];
//                                   *(uint8_t*)Upointer++=cNotiTemp[i2][(ch)];
//                                   checksum+=cNotiTemp[i2][(ch)+1];
//                        }
//
//                     }
//
//                     *(uint8_t*)Upointer++=ETX;
//                      checksum+=ETX;
//
//                      *(uint8_t*)Upointer++=checksum;
//                      *(uint8_t*)Upointer++=EOT;
//
//
//                 ++attempts;
//                    if(i3==1){
//    //status = GATT_Indication(item->connHandle,(attHandleValueInd_t *) &notification,FALSE,0);
//            status = GATT_Notification(item->connHandle, &notification, FALSE);
//                        if (status != SUCCESS) {
//                            for (i2=0;i2<=9;i2++){
//                                for (jx=0;jx<8;jx++){
//                                    if(jx==3){
//                                        *(uint8_t*)EUpointer++=status & 0xff;//cSpiRxBuffer2[(ch)+2];
//                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)+1];
//                                        *(uint8_t*)EUpointer++=0x00;//cSpiRxBuffer2[(ch)];
//
//                                    }else{
//
//                                        *(uint8_t*)EUpointer++=clocal[jx];//cSpiRxBuffer2[(ch)+2];
//                                        *(uint8_t*)EUpointer++=clocal[jx+1];//cSpiRxBuffer2[(ch)+1];
//                                        *(uint8_t*)EUpointer++=clocal[jx+2];//cSpiRxBuffer2[(ch)];
//
//                                    }
//                                }
//                            }
//
//                            *(uint8_t*)EUpointer++=ETX;
//                            *(uint8_t*)EUpointer++=checksum;
//                            *(uint8_t*)EUpointer++=EOT;
//                             UART_write(uart, cErrorframe, sizeof(cErrorframe));
//                            //Log_info1("Notification: status=%d", status);
//                            GATT_bm_free((gattMsg_t *) &notification, ATT_HANDLE_VALUE_NOTI);
//                            imiliTimeCopy=imiliTime; //real data loss
//
//                        } else {
//
//                            imiliTimeCopy=imiliTime;
//                             UART_write(uart, clocal, sizeof(clocal));
//                            //iDrdyCount=0;
//                        }
//                   }
//                }
//            //}
//            return 0;
//
//            #endif
//
//            #if UARTDUMP
//
//                    ix=ScanBuffer.iCount;
//                    in =ScanBuffer.iIptr;
//                    io =ScanBuffer.iOptr;
//                    if (ix<=9) return 0;
//                    clocal[0]=0x1;
//                    clocal[1]=0x2;
//                    clocal[2]=0x3;
//                    i=1;
//
//
//                        for (i2=0;i2<=9;i2++){
//
//                           i1=GetDataFromScanBuffer();
//                               if (i1!=0){
//                                   for (jx=1;jx<9;jx++){
//                                       if(jx==1)ch=LEAD_1;
//                                       if(jx==2)ch=LEAD_2;
//                                       if(jx==3)ch=V1;
//                                       if(jx==4)ch=V2;
//                                       if(jx==5)ch=V3;
//                                       if(jx==6)ch=V4;
//                                       if(jx==7)ch=V5;
//                                       if(jx==8)ch=V6;
//
//
//                                       clocal[(i*3)+2]=0xff;//cSpiRxBuffer2[(ch)+2];
//                                       clocal[(i*3)+1]=0x55;//cSpiRxBuffer2[(ch)+1];
//                                       clocal[(i*3)]=0xaa;//cSpiRxBuffer2[(ch)];
//
//
//
//                                }
//                               }
//
//                            i++;
//                        }
//                        UART_write(uart, &clocal, sizeof(clocal));
//                 return 0;
//
//            #endif
//
//}
//
//





// to check the  indication

//                status = (*pfnReadAttrCB)( item->connHandle, notification.pValue, &notification.len,
//                0, PAYLOAD_BYTES, GATT_LOCAL_READ );


//                if ( status == SUCCESS )
//                {
//                noti.handle = pAttr->handle;
//                if ( cccValue & GATT_CLIENT_CFG_NOTIFY )
//                {
//                status = GATT_Notification( connHandle, &noti, authenticated );
//                }
//                else // GATT_CLIENT_CFG_INDICATE
//                {
//                status = GATT_Indication( connHandle, (attHandleValueInd_t *)&noti,
//                authenticated, taskId );
//                }
//                }
//                if ( status != SUCCESS )
//                {
//                GATT_bm_free( (gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI );
//                }
//                }
//                else
//                {
//                status = bleNoResources;
//                }
//
//


