/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the CC2652R1FRGZ
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC2652R1FRGZ
#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== CCFG ========
 */


/*
 *  ======== AESCCM ========
 */

extern const uint_least8_t                  CONFIG_AESCCM0_CONST;
#define CONFIG_AESCCM0                      0
#define CONFIG_TI_DRIVERS_AESCCM_COUNT      1


/*
 *  ======== AESCTR ========
 */

extern const uint_least8_t                  CONFIG_AESCTR_0_CONST;
#define CONFIG_AESCTR_0                     0
#define CONFIG_TI_DRIVERS_AESCTR_COUNT      1


/*
 *  ======== AESCTRDRBG ========
 */

extern const uint_least8_t                      CONFIG_AESCTRDRBG_0_CONST;
#define CONFIG_AESCTRDRBG_0                     0
#define CONFIG_TI_DRIVERS_AESCTRDRBG_COUNT      1


/*
 *  ======== AESECB ========
 */

extern const uint_least8_t                  CONFIG_AESECB0_CONST;
#define CONFIG_AESECB0                      0
#define CONFIG_TI_DRIVERS_AESECB_COUNT      1


/*
 *  ======== ECDH ========
 */

extern const uint_least8_t              CONFIG_ECDH0_CONST;
#define CONFIG_ECDH0                    0
#define CONFIG_TI_DRIVERS_ECDH_COUNT    1


/*
 *  ======== GPIO ========
 */

/* DIO6 */
extern const uint_least8_t              AFE_START_CONST;
#define AFE_START                       0
/* DIO7 */
extern const uint_least8_t              AFE_RESET_CONST;
#define AFE_RESET                       1
/* DIO10 */
extern const uint_least8_t              AFE_PRDWN_CONST;
#define AFE_PRDWN                       2
/* DIO27 */
extern const uint_least8_t              LED1_CONST;
#define LED1                            3
/* DIO3 */
extern const uint_least8_t              AFE_CS_CONST;
#define AFE_CS                          4
/* DIO4 */
extern const uint_least8_t              AFE_DRDY_CONST;
#define AFE_DRDY                        5
/* DIO28 */
extern const uint_least8_t              nvs_CONST;
#define nvs                             6
#define CONFIG_TI_DRIVERS_GPIO_COUNT    7

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== NVS ========
 */

extern const uint_least8_t              CONFIG_NVSINTERNAL_CONST;
#define CONFIG_NVSINTERNAL              0
/*
 *  MOSI: DIO0
 *  MISO: DIO2
 *  SCLK: DIO1
 *  SS: DIO28
 */
extern const uint_least8_t              CONFIG_NVSEXTERNAL_CONST;
#define CONFIG_NVSEXTERNAL              1
#define CONFIG_TI_DRIVERS_NVS_COUNT     2


/*
 *  ======== PIN ========
 */
#include <ti/drivers/PIN.h>

extern const PIN_Config BoardGpioInitTable[];

/* Parent Signal: AFE_START GPIO Pin, (DIO6) */
#define CONFIG_PIN_4                   0x00000006
/* Parent Signal: AFE_RESET GPIO Pin, (DIO7) */
#define CONFIG_PIN_5                   0x00000007
/* Parent Signal: AFE_PRDWN GPIO Pin, (DIO10) */
#define CONFIG_PIN_6                   0x0000000a
/* Parent Signal: LED1 GPIO Pin, (DIO27) */
#define CONFIG_PIN_7                   0x0000001b
/* Parent Signal: AFE_CS GPIO Pin, (DIO3) */
#define CONFIG_PIN_12                   0x00000003
/* Parent Signal: AFE_DRDY GPIO Pin, (DIO4) */
#define CONFIG_PIN_8                   0x00000004
/* Parent Signal: CONFIG_SPI_0 SCLK, (DIO1) */
#define CONFIG_PIN_1                   0x00000001
/* Parent Signal: CONFIG_SPI_0 MISO, (DIO2) */
#define CONFIG_PIN_2                   0x00000002
/* Parent Signal: CONFIG_SPI_0 MOSI, (DIO0) */
#define CONFIG_PIN_3                   0x00000000
/* Parent Signal: nvs GPIO Pin, (DIO28) */
#define CONFIG_PIN_0                   0x0000001c
#define CONFIG_TI_DRIVERS_PIN_COUNT    10




/*
 *  ======== SPI ========
 */

/*
 *  MOSI: DIO0
 *  MISO: DIO2
 *  SCLK: DIO1
 */
extern const uint_least8_t              CONFIG_SPI_0_CONST;
#define CONFIG_SPI_0                    0
#define CONFIG_TI_DRIVERS_SPI_COUNT     1


/*
 *  ======== TRNG ========
 */

extern const uint_least8_t              CONFIG_TRNG_0_CONST;
#define CONFIG_TRNG_0                   0
#define CONFIG_TI_DRIVERS_TRNG_COUNT    1


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
