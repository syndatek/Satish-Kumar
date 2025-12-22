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

 ADC_Handle   adc;
 ADC_Params   adc_params;
 int_fast16_t res;



uint16_t adcValue0;

uint32_t adcValue0MicroVolt;



/*void SydanTekReadadc(void)
 *
 *  ======== threadFxn0 ========
 *  Open an ADC instance and get a sampling result from a one-shot conversion.
 */
uint16_t SydantekReadadc(void)
{

    /* Blocking mode conversion */
    res = ADC_convert(adc, &adcValue0);

    if (res == ADC_STATUS_SUCCESS) {

        //adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);
        //return (adcValue0MicroVolt);

        return(adcValue0);

    }
    else {
        adcValue0MicroVolt=0;
        return (NULL);

    }

    //ADC_close(adcValue0MicroVolt);


}

/*********************************************************************
void SydantekAdc_Init(void)


**********************************************************************/
void SydantekAdc_Init(void)
{
    /* Call driver init functions */
    ADC_init();
   // Display_init();
    ADC_Params_init(&adc_params);
    adc = ADC_open(CONFIG_ADC_0, &adc_params);

    if (adc == NULL) {
//        Display_printf(display, 0, 0, "Error initializing CONFIG_ADC_0\n");
        while (1);
    }


   return (NULL);
}









