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
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
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
//#include  <CC2640R2_LAUNCHXL.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <uartlog/UartLog.h>
#include <icall.h>
#include <icall_ble_api.h>
//#include <sydantek.h>


#include <services/sydantek.h>


// 251 - 3-byte ATT notification header, 4 byte L2CAP header.
#define PAYLOAD_BYTES         244

typedef struct __attribute__((__packed__)) {
    uint16_t version;  // has to be 1
    uint16_t leads;    // 1, 2 or 8
    uint16_t limb_hz;  // 500, 1000, 2000
    uint16_t chest_hz; // 500, 1000, 2000
} params_t;

#define SERVICE_INDEX         0
#define PARAMS_INDEX          1
#define NOTIFY_INDEX          2

static CONST uint8_t service_uuid[] = { SYDANTEK_UUID(SERVICE_INDEX) };
static CONST uint8_t params_uuid[] = { SYDANTEK_UUID(PARAMS_INDEX) };
static CONST uint8_t notify_uuid[] = { SYDANTEK_UUID(NOTIFY_INDEX) };

// Characteristic values & configuration.
static params_t params = { 1, 2, 500, 500 };
static uint8_t notify[1];  // not really used.
static gattCharCfg_t* notify_config;






int  iflag=1;

char fDRYCount;





#define CHANNEL_NO 2//Afe channelnumber changed from 1---8 0 array  contains status byte
//----------------------------------------------------------------------------
#define ID_DISPLAY_COUNT 20 //1 to 127 donot exceed the number

#define Num_Packet    4
#define SPIDMAABORT_T  20//which is 40 milisecond
//-----------------------------Filters Enabling---------------
#define FILTER_DC_REQUIRED  0// set this to 1 or 0 depending on DC filter is required or not
#define FILTER_50_REQUIRED  0// set this to 1 or 0 depending on 50Hz filter is required or not
#define FILTER_MVA_REQUIRED 0 // set this to 1 or 0 depending on MVA filter is required or not
#define FILTER_05_REQUIRED  0 // set this to 1 or 0 depending on 05 Hz filter is required or not
 //-----------------------------Filters Enabling---------------



//#define enbling  singal internal and external not enabled
#define INTERNAL_TEST_SIGNAL    0x10    //0x10 is test signal enabled
#define INTERNAL_TEST_SIGNAL_CH 0x15    //Internal Test Tone gain set 1

#define EXTERNAL_TEST_SIGNAL 0x00     //0x10 is test signal enabled



#define EXTERNAL_SIG         0x10  //external gain set 1
#define CH_GAIN              0x10

//#define EXTERNAL_SIG         0x60  //gain set to 12

//#define EXTERNAL_SIG         0x00  //gain set to six

//#define EXTERNAL_SIG         0x30  //gain set to 2


#define NOISE_MEASUREMENT    0x11  //CHnSET to  01 for noise Measurement cal
#define VOLTAGE_MEASUREMENT  0x13  //CHnSET Voltage Voltage Meaurement
#define TEMP_MEASUREMENT     0x14  //CHnSET Voltage TEMP Meaurement
#define CH_PDWN              0x80

#define CH2_P_RLD           0x06  // channel 2 positive to RLD Driver
#define CH2_N_RLD           0x07  // channel 2 positive to RLD Driver

#define RLD_AMP_ENABLED     0x40  //RLD Ampilfier enabled



#define SIMULATED_DATA_OUTPUT  1  //IF we wanted body signal that is simulated with DC Compnent and 50 hz removed can be used

int iToggle;
int iSPIDMAAbortCnt;

//char cTempBfr[6][27]; //instaneously frames are loaded
uint8_t cTempBfr[8][18]; // channel on 1 -8 can be18 bytes to be collected to get Send the data
uint8_t cTempBfrCopy[8][18]; // channel on 1 -8 can be18 bytes to be collected to get Send the data
char cPenidingEvtf=0;
int iPacketNumber1Copy;



int iBodySampleCount=0;

static uint16  ECG_Sample_Count1=0,ECG_Sample_Count2=0,ECG_Sample_Count3=0;
//-------------------SLP 15 Feb 16
uint8 delay_count;
uint8 LED_State=1;
uint8 Board_SPI=0;

PIN_State pinState;
PIN_Handle hCpuPins;

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



//--------------------------


//uint8_t txBuf[27] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27};    // Transmit buffer
uint8_t txBuf[27];
uint16_t Sampleindex=0;

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
uint8_t cExternalbuffer[27];
SPI_Handle spiHandle;
uint8 SPI0=0;
uint8 SPI1=1;
uint8_t iSPIStatus;
int iPacketNumber=0;
int iPacketNumber1[8];

char cIDCountDisplay=0,cFailcount=0; //displayAfe ad1298 id to ble for 20 times
unsigned char cAFE_ID=0xff; //default ot 0xff
float f1,f2;

//static ICall_Semaphore sem;
static ICall_SyncHandle syncEvent;





//-----------
uint16_t iDataPointer;
int iInGetData,iInNextScanBuffer;
//------------


inline void OneDataNotificationCorrected160bytes(void);
int AFEinit3(void);



//---------------
/*********************************************************************
 * EXTERNAL VARIABLES
 */
 SPI_Handle      handle;
 //SPI_Params      params;
 SPI_Transaction transaction;
 SPI_Params      spi_params;
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

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // Event header
} heartRateEvt_t;

bool bDataReadinProgress;

char chr0x33;
extern int iIntCount;
int  iSpiCount;
uint8_t *ptrbuf;
char cSpiTxferCmplete;
char cBleTxferCmplete;
 //void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId);
union {
    long l1;
    char c1[4];
} ldata;







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
//int ReadDataFromAFEasynchronous(void);
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
    PIN_setOutputValue(hCpuPins,AFE_CS,0);
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
    PIN_setOutputValue(hCpuPins,AFE_CS,1);
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

    PIN_setOutputValue(hCpuPins,0,0);//CONFIG_SPI_0 MOSI
    PIN_setOutputValue(hCpuPins,1,0);//CONFIG_SPI_0 SCLK
    PIN_setOutputValue(hCpuPins,3,0);//CONFIG_SPI_0 CS
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
    hCpuPins= PIN_open(&pinState, &BoardGpioInitTable[0]);
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
        PIN_setOutputValue(hCpuPins,7,0);

        DelayuS(100000);
        PIN_setOutputValue(hCpuPins,7,1);
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

    PIN_setOutputValue(hCpuPins,6,0);       // AFE start is doen by sending command
                                                    // For command to be accepted, this line is to be kept low
    DelayuS(1);
    //PIN_setOutputValue(hCpuPins,AFE_START,1);
    //DelayuS(100);
    cSpiTxBuffer[0] = AFE_OPCODE_START;
    SendtoAFE (handle,1,0);                                 //restart continuous read

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

    b1=SPI_transfer(handle, &transaction);
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

    b1=SPI_transfer(handle, &transaction);
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

    PIN_setOutputValue(hCpuPins,10,1);
    PIN_setOutputValue(hCpuPins,7,1);
    DelayuS(100);
    PIN_setOutputValue(hCpuPins,7,0);
    DelayuS(1000);
    PIN_setOutputValue(hCpuPins,7,1);
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
uint8_t* ptr;
int i,j,k;
    k=0;
    ptr=0;
    //if (iInGetData==1) return 0;
    iInNextScanBuffer=1;
    i=ScanBuffer.iIptr;
    ptr=&(ScanBuffer.ScanData[i].cOneScanData[0]);

    j=ScanBuffer.iCount;
    if (j>SCAN_BUF_SIZE) {
        k=1;
        iInNextScanBuffer=0;
        ScanBuffer.iOverrunFlag=1;
        return 0;
    }

    if (j<0) {
        ResetScanBuffer();
        i=ScanBuffer.iIptr;
        ptr=&(ScanBuffer.ScanData[i].cOneScanData[0]);
        ptrbuf=ptr;
        ScanBuffer.iOverrunFlag=2;
        iInNextScanBuffer=0;
        return 1;
    }



    j++;
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
    int i;
    iSpiCount++;
    bDataReadinProgress=1;
    //ReadDataFromAFEasynchronous();
    //iPacketNumber++;
    //Semaphore_post(sem);
}


/**----------------------------------------------------------------
static void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId)
Call back function - gets control on DRDY interrupt
input: none
returns:nothing
----------------------------------------------------------------*/
volatile void DRDY_Callback(PIN_Handle hPin, PIN_Id pinId)
{
  extern Swi_Handle hswiDRDY;
  iSPIDMAAbortCnt=SPIDMAABORT_T;
  Swi_post(hswiDRDY);
}


/**----------------------------------------------------------------
Void spiCallback (SPI_Handle handle, SPI_Transaction *transaction)

------------------------------------------------------------------*/
void SPICallbackFunction (SPI_Handle handle, SPI_Transaction *pTransaction)
{

    void *ptr;
    int i,j;
   // PIN_setInterrupt(hCpuPins, AFE_DRDY|PIN_IRQ_DIS); // disable drdyinterrupt
    if (transaction.status==SPI_TRANSFER_COMPLETED){
        cSpiTxferCmplete=1 ;// indicate that process buffer increment is in processs
        chr0x33= (iSpiCount & 0xff);
        j=ScanBuffer.iCount;
        if (j<SCAN_BUF_SIZE) {
            i=ScanBuffer.iIptr; //move in pointer forward
            i++;
            if (i>=SCAN_BUF_SIZE) i=0;
             ScanBuffer.iIptr=i;
             ScanBuffer.iCount++;
        }
        cSpiTxferCmplete=0; //// indicates that process buffer increment is compeleted
        //Semaphore_post(syncEvent);
        Event_post(syncEvent, 1);
    } else {
        chr0x33=0x44;
    }
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
int i,j;
//for(j=0;j<10;j++){ //init is done 10 times

SPI_init();

//-------SPI initalisation-----------
SPI_Params_init(&spi_params);                       // initialises parameters to defaults
spi_params.bitRate  = 500000;
spi_params.transferTimeout=100;
spi_params.transferMode=SPI_MODE_BLOCKING;              //SPI_MODE_CALLBACK;
//params.transferCallbackFxn=SPICallbackFunction;
spi_params.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
spi_params.mode        = SPI_MASTER;
handle = SPI_open(0, &spi_params);              // initialise SPI

spiHandle=handle;
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
cSpiTxBuffer[i++] = 0x86;
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
cSpiTxBuffer[i++] =0x10;//NOISE_MEASUREMENT|CH_PDWN;//0x15;//NOISE_MEASUREMENT|CH_PDWN;//0x00;//0x10;//NOISE_MEASUREMENT|CH_PDWN;//NOISE_MEASUREMENT;    //Noise measurement

i=SendtoAFE2 (i,40);

i=0;
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
cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_RLD_SENSP;//sense 7
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] = 0x01;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
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
cSpiTxBuffer[i++] = 0x01;    ;//NOISE_MEASUREMENT|CH_PDWN;//INTERNAL_TEST_SIGNAL_CH; //EXTERNAL_SIG;//NOISE_MEASUREMENT;//internal test signal                                   //   gain is 3 and test signal is given as input for mux
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
i=0;


cSpiTxBuffer[i++] = AFE_OPCODE_WREG |AFE_WCT2;
cSpiTxBuffer[i++] = 0x00;
cSpiTxBuffer[i++] =0x40;// 0x40;=LA   //LL=0x82                   //
i=SendtoAFE2 (i,40);

i=0;
cSpiTxBuffer[0] = AFE_OPCODE_RREG | AFE_ID;
cSpiTxBuffer[1] = 19;                                   // we need to read one ID register
cSpiTxBuffer[2] = 0xFF;
i=SendtoAFE2 (2,40);
//i=0;

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


//cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
//i=SendtoAFE2 (1,0);

//-------------------------------------------------------------------------------------------------------------------------


                        //Read ID byte. It should be 0x92

//cSpiTxBuffer[i++] = AFE_OPCODE_WREG | AFE_CONFIG2;
//cSpiTxBuffer[i++] = 0x00;
//cSpiTxBuffer[i++] = 0x15;             //10-AC,13-DC,15- double freq & amplitude internal test signal

//System_printf("Initialisation Complete");


ResetScanBuffer();
cSpiTxferCmplete=0;

DelayuS(10);
DelayuS(10);
DelayuS(10);

Error_init(&eb);
Swi_Params_init(&swiParams);


hswiDRDY = Swi_create((ti_sysbios_knl_Swi_FuncPtr)swiDRDYFunction, &swiParams,&eb);     // this sofwtare interrupt will be generated by DRDY HWI

if (hswiDRDY == NULL) {
    return AFE_INIT_ERROR;                          // eror occured while creating SWI
}
PIN_registerIntCb(hCpuPins, DRDY_Callback);
PIN_setConfig(hCpuPins, PIN_BM_IRQ, AFE_DRDY   | PIN_IRQ_NEGEDGE);
PIN_setInterrupt(hCpuPins, AFE_DRDY|PIN_IRQ_NEGEDGE);



//---------------------------------------------------------

AFEstart();
DelayuS(10);

cSpiTxBuffer[0] = AFE_OPCODE_RDATAC;
SendtoAFE (handle,1,0);                                 //restart continuous read

AFEChipUnSelect();
DelayuS(10);
AFEChipSelect();

SPI_close(handle);

SPI_Params_init(&params);
spi_params.bitRate  =4000000;  //4mhz
spi_params.transferTimeout=1000;
spi_params.transferMode=SPI_MODE_CALLBACK;//SPI_MODE_BLOCKING
spi_params.transferCallbackFxn=SPICallbackFunction;
spi_params.frameFormat = SPI_POL0_PHA1;             // this is Motorola format
spi_params.mode        = SPI_MASTER;
handle = SPI_open(SPI0, &spi_params);               // initialise SPI

DelayuS(10);
cInterprocessSem=0;

return AFE_INIT_SUCCESS;
}







/*********************************************************************
 * @fn OneDataNotification
 *
 * @brief   Prepare and send a heart rate measurement notification.
 *
 * @return  none
 **********************************************************************/
inline void OneDataNotificationCorrected160bytes(void)
{

//    uint8_t loop,i,j,k;
//     uint16_t Sample_data;
//     uint32_t  lTime;
//     uint8 Reserve1=0;
//     uint8 Reserve2=0;
//     union {
//         uint32_t l1;
//         char c1[4];
//     } uGettime;
//     uint8_t *p ;
//     uint8_t flags ;
//     uint8_t *ptr ;
//     int i1=0,i2=0,i3=0,i4=0,ch1;
//
//     attHandleValueNoti_t heartRateMeas;
//
//    i=ScanBuffer.iCount;
//    if (i<6) return;          //no data to send as message
//
//     j=(i/6);
//     for (i=0;i<j;i++){
//         heartRateMeas.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI,
//                                              HEARTRATE_MEAS_LEN, NULL);
//         if (heartRateMeas.pValue != NULL) {
//            p= heartRateMeas.pValue;
//            flags= heartRateflags[flagsIdx];
//            ptr= heartRateMeas.pValue;
//
//            if (ScanBuffer.iCount>=6){             // data is not yet available, so wait for it
//                i3=1;
//                for (i2=0;i2<6;i2++){
//                    i1=GetDataFromScanBuffer();
//
//                    if (i1!=0){
//                            cTempBfr[0][i2*3]=cSpiRxBuffer2[3];//contain channel 1 ist byte the 3bytes
//                            cTempBfr[0][(i2*3)+1]=cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
//                            cTempBfr[0][(i2*3)+2]=cSpiRxBuffer2[5];//contain the 3bytes
//
//                            cTempBfr[1][i2*3]=cSpiRxBuffer2[6];//contain the 3bytes
//                            cTempBfr[1][(i2*3)+1]=cSpiRxBuffer2[7];//contain the 3bytes
//                            cTempBfr[1][(i2*3)+2]=cSpiRxBuffer2[8];//contain the 3bytes
//
//                            cTempBfr[2][i2*3]=cSpiRxBuffer2[9];//contain the 3bytes
//                            cTempBfr[2][(i2*3)+1]=cSpiRxBuffer2[10];//contain the 3bytes
//                            cTempBfr[2][(i2*3)+2]=cSpiRxBuffer2[11];//contain the 3bytes
//
//                            cTempBfr[3][i2*3]=cSpiRxBuffer2[12];//contain the 3bytes
//                            cTempBfr[3][(i2*3)+1]=cSpiRxBuffer2[13];//contain the 3bytes
//                            cTempBfr[3][(i2*3)+2]=cSpiRxBuffer2[14];//contain the 3bytes
//
//
//                            cTempBfr[4][i2*3]=cSpiRxBuffer2[15];//contain the 3bytes
//                            cTempBfr[4][(i2*3)+1]=cSpiRxBuffer2[16];//contain the 3bytes
//                            cTempBfr[4][(i2*3)+2]=cSpiRxBuffer2[17];//contain the 3bytes
//
//
//                            cTempBfr[5][i2*3]=cSpiRxBuffer2[18];//contain the 3bytes
//                            cTempBfr[5][(i2*3)+1]=cSpiRxBuffer2[19];//contain the 3bytes
//                            cTempBfr[5][(i2*3)+2]=cSpiRxBuffer2[20];//contain the 3bytes
//
//
//                            cTempBfr[6][i2*3]=cSpiRxBuffer2[21];//contain the 3bytes
//                            cTempBfr[6][(i2*3)+1]=cSpiRxBuffer2[22];//contain the 3bytes
//                            cTempBfr[6][(i2*3)+2]=cSpiRxBuffer2[23];//contain the 3bytes
//
//                            cTempBfr[7][i2*3]=cSpiRxBuffer2[24];//contain the 3bytes
//                            cTempBfr[7][(i2*3)+1]=cSpiRxBuffer2[25];//contain the 3bytes
//                            cTempBfr[7][(i2*3)+2]=cSpiRxBuffer2[26];//contain the 3bytes
//
//                            ///////////////////////////////////////////////////////////
//
////                            cTempBfr[0][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[0][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[0][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////                            cTempBfr[1][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[1][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[1][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////                            cTempBfr[2][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[2][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[2][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////                            cTempBfr[3][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[3][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[3][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////
////                            cTempBfr[4][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[4][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[4][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////
////                            cTempBfr[5][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[5][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[5][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////
////                            cTempBfr[6][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[6][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[6][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
////                            cTempBfr[7][i2*3]=0xaa;//contain channel 1 ist byte the 3bytes
////                            cTempBfr[7][(i2*3)+1]=0x00;//cSpiRxBuffer2[4];//contain channel 2 the 2 bytes
////                            cTempBfr[7][(i2*3)+2]=0x01;//cSpiRxBuffer2[5];//contain the 3bytes
////
//
//
//
//                    }
//                }
////#######################################################CH_1####################################################
//               for(i4=0;i4<8;i4++){
//
//                   *ptr=(iPacketNumber & 0xff00)>>8;            // store MSb first and thn LSB
//                    ptr++;
//                   *ptr=(iPacketNumber & 0xff);
//                    ptr++;
//                    for (i2=0;i2<6;i2++){
//                        *ptr= cTempBfr[i4][(i2*3)];
//                         ptr++;
//                         *ptr= cTempBfr[i4][(i2*3)+1];
//                          ptr++;
//                         *ptr= cTempBfr[i4][(i2*3)+2];
//                          ptr++;
//
//                    }
//               }
//
//               //iPacketNumber++;
//
//          }
//
//           if (i3==1){
//               i3=0;
//               heartRateMeas.len = DATA_PACKET_LEN;    //(uint8)(p- heartRateMeas.pValue);
//               // Send notification.
//               ch1=HeartRate_MeasNotify(gapConnHandle, &heartRateMeas);
//               if (ch1 != SUCCESS) { // check for the fail count
//                   if(ch1==0x04){
//                       cPenidingEvtf=1;
//                       //iPacketNumber|=0x8000;
//                   }
//                   iPacketNumber=ch1;
//
//                   //cfailCount++;
//                 //if(cfailCount>3 ){
//                   //  cfailCount=0;
//                         GATT_bm_free((gattMsg_t *)&heartRateMeas, ATT_HANDLE_VALUE_NOTI);
//                         //iPacketNumber=ch1;
//                        // iPacketNumber=iPacketNumbe-1;
//                 //}
//               }else{
//                   iPacketNumber++;
//               }
//
//           }
//
//
//        }
//       }
//
//
//
//
//
}












































void Sydantek_ProcessTimerEvent() {
    static uint32_t count = 0;
    static uint32_t attempts = 0;
    static uint32_t successes = 0;
    static uint32_t serial = 0xa0000000;

    if (++count % 1000 == 0 || count == 1) {
        ICall_heapStats_t stats;
        ICall_getHeapStats(&stats);
        Log_info3("@ heap: %u total, %u free, %u largest",
                  stats.totalSize, stats.totalFreeSize, stats.largestFreeSize);
        Log_info3("count %d, attempts %d, successes %d", count, attempts, successes);
    }

    attHandleValueNoti_t notification;
    bStatus_t status;

    gattAttribute_t* attribute = GATTServApp_FindAttr(
            attributes, GATT_NUM_ATTRS(attributes), notify);
    notification.handle = attribute->handle;

    for (int i = 0; i < linkDBNumConns; ++i) {
        gattCharCfg_t *item = notify_config + i;
        if (item->connHandle == LINKDB_CONNHANDLE_INVALID) continue;
        if (item->value == GATT_CFG_NO_OPERATION) continue;

        for (int k = 0; k < 6; ++k) {
            notification.pValue = (uint8_t*) GATT_bm_alloc(
                    item->connHandle, ATT_HANDLE_VALUE_NOTI, PAYLOAD_BYTES,
                    &notification.len);
            if (!notification.pValue) break;

            uint8_t* pointer = notification.pValue;
            *(uint32_t*) pointer = serial;
            *(uint32_t*) (pointer + notification.len - 4) =0x00; //0xaa;//serial;

            ++attempts;
            status = GATT_Notification(item->connHandle, &notification, FALSE);
            if (status != SUCCESS) {
                Log_info1("Notification: status=%d", status);
                GATT_bm_free((gattMsg_t *) &notification, ATT_HANDLE_VALUE_NOTI);
            } else {
                ++successes;
                ++serial;
            }
        }
    }
}

