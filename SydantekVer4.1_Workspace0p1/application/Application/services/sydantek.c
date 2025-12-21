/*
 * sydantek.c
 *
 * Copyright (c) 2020 and onwards, Carditek Medical Devices, Pvt. Ltd.
 * All rights reserved.
 */

#include <uartlog/UartLog.h>
#include <icall.h>
#include <icall_ble_api.h>

#include "sydantek.h"

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




void Sydantek_ProcessTimerEvent() {
    static uint32_t count = 0;
    static uint32_t attempts = 0;
    static uint32_t successes = 0;
    static uint32_t serial = 0xa0000000;

//    if (++count % 1000 == 0 || count == 1) {
//        ICall_heapStats_t stats;
//        ICall_getHeapStats(&stats);
//        Log_info3("@ heap: %u total, %u free, %u largest",
//                  stats.totalSize, stats.totalFreeSize, stats.largestFreeSize);
//        Log_info3("count %d, attempts %d, successes %d", count, attempts, successes);
//    }

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
            *(uint32_t*) (pointer + notification.len - 4) = serial;

            ++attempts;
            status = GATT_Notification(item->connHandle, &notification, FALSE);
            if (status != SUCCESS) {
//                Log_info1("Notification: status=%d", status);
                GATT_bm_free((gattMsg_t *) &notification, ATT_HANDLE_VALUE_NOTI);
            } else {
                ++successes;
                ++serial;
            }
        }
    }
}
