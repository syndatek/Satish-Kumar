/*
 * sydantek.h
 *
 * Copyright (c) 2020 and onwards, Carditek Medical Devices, Pvt. Ltd.
 * All rights reserved.
 */

#ifndef APPLICATION_SERVICES_SYDANTEK_H_
#define APPLICATION_SERVICES_SYDANTEK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bcomdef.h>

// UUID "a965db41-5e30-ad9e-fe47-02a5822878 ??"; service at 0, ...
#define SYDANTEK_UUID(id) (id) /* 0x55 */, 0x78, 0x28, 0x82, 0xa5, 0x02, \
    0x47, 0xfe, 0x9e, 0xad, 0x30, 0x5e, 0x41, 0xdb, 0x65, 0xa9

extern bStatus_t Sydantek_AddService(uint8_t rspTaskId);

extern void Sydantek_ProcessTimerEvent();

#define SYDANTEK_TIMER_PERIOD_MSECS 5

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_SERVICES_SYDANTEK_H_ */
