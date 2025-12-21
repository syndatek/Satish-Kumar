/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== adcsinglechannel.c ========
 */
/******************************************************************************
 Includes
 *****************************************************************************/
//#include <openthread/config.h>
//#include <openthread-core-config.h>


//


#include <stdint.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* ADC sample count */
#define ADC_SAMPLE_COUNT  (10)

#define THREADSTACKSIZE   (768)

/* ADC conversion result variables */
uint16_t adcValue0;
uint32_t adcValue0MicroVolt;
uint16_t adcValue1[ADC_SAMPLE_COUNT];
uint32_t adcValue1MicroVolt[ADC_SAMPLE_COUNT];

static Display_Handle display;

//Task_Struct   AdcStruct;
char AdcStructStack[THREADSTACKSIZE];



/*
 *  ======== threadFxn0 ========
 *  Open an ADC instance and get a sampling result from a one-shot conversion.
 */
void *threadFxn0(void *arg0)
{
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(CONFIG_ADC_0, &params);

    if (adc == NULL) {
        Display_printf(display, 0, 0, "Error initializing CONFIG_ADC_0\n");
        while (1);
    }

    /* Blocking mode conversion */
    res = ADC_convert(adc, &adcValue0);

    if (res == ADC_STATUS_SUCCESS) {

        adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);

        Display_printf(display, 0, 0, "CONFIG_ADC_0 raw result: %d\n", adcValue0);
        Display_printf(display, 0, 0, "CONFIG_ADC_0 convert result: %d uV\n",
            adcValue0MicroVolt);
    }
    else {
        Display_printf(display, 0, 0, "CONFIG_ADC_0 convert failed\n");
    }

    ADC_close(adc);

    return (NULL);
}

///*
// *  ======== threadFxn1 ========
// *  Open a ADC handle and get an array of sampling results after
// *  calling several conversions.
// */
//void *threadFxn1(void *arg0)
//{
//    uint16_t     i;
//    ADC_Handle   adc;
//    ADC_Params   params;
//    int_fast16_t res;
//
//    ADC_Params_init(&params);
//    adc = ADC_open(CONFIG_ADC_1, &params);
//
//    if (adc == NULL) {
//        Display_printf(display, 0, 0, "Error initializing CONFIG_ADC_1\n");
//        while (1);
//    }
//
//    for (i = 0; i < ADC_SAMPLE_COUNT; i++) {
//        res = ADC_convert(adc, &adcValue1[i]);
//
//        if (res == ADC_STATUS_SUCCESS) {
//
//            adcValue1MicroVolt[i] = ADC_convertRawToMicroVolts(adc, adcValue1[i]);
//
//            Display_printf(display, 0, 0, "CONFIG_ADC_1 raw result (%d): %d\n", i,
//                           adcValue1[i]);
//            Display_printf(display, 0, 0, "CONFIG_ADC_1 convert result (%d): %d uV\n", i,
//                adcValue1MicroVolt[i]);
//        }
//        else {
//            Display_printf(display, 0, 0, "CONFIG_ADC_1 convert failed (%d)\n", i);
//        }
//    }
//
//    ADC_close(adc);
//
//    return (NULL);
//}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    ADC_init();
    Display_init();


   return (NULL);
}
//

/*
 *  ======== mainThread ========
 *  ======== mainThread ========
 *  ======== mainThread ========
 *  ======== mainThread ========
 *  ======== mainThread ========
 *  ======== mainThread ========
 *  ======== mainThread ========
 */

void Task_BatteryInit()
{

//    Task_Params taskParams;
//    Task_Params_init(&taskParams);
//    taskParams.priority = 1;
//    taskParams.stackSize = THREADSTACKSIZE;
//    taskParams.stack = &task0Stack;
//    taskParams.instance->name = "echo";
//    Task_construct(&task0Struct, (Task_FuncPtr)mainThread, &taskParams, NULL);

}











