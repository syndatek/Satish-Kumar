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

/* DIO3 */
extern const uint_least8_t              CONFIG_GPIO_BTN1_CONST;
#define CONFIG_GPIO_BTN1                0
/* DIO4 */
extern const uint_least8_t              CONFIG_GPIO_BTN2_CONST;
#define CONFIG_GPIO_BTN2                1
/* DIO6 */
extern const uint_least8_t              CONFIG_GPIO_RLED_CONST;
#define CONFIG_GPIO_RLED                2
/* DIO7 */
extern const uint_least8_t              CONFIG_GPIO_GLED_CONST;
#define CONFIG_GPIO_GLED                3
/* DIO10 */
extern const uint_least8_t              CONFIG_GPIO_0_CONST;
#define CONFIG_GPIO_0                   4
#define CONFIG_TI_DRIVERS_GPIO_COUNT    5

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
 *  SS: DIO10
 */
extern const uint_least8_t              CONFIG_NVSEXTERNAL_CONST;
#define CONFIG_NVSEXTERNAL              1
#define CONFIG_TI_DRIVERS_NVS_COUNT     2


/*
 *  ======== PIN ========
 */
#include <ti/drivers/PIN.h>

extern const PIN_Config BoardGpioInitTable[];

/* Parent Signal: CONFIG_DISPLAY_UART RX, (DIO23) */
#define CONFIG_PIN_UART_RX                   0x00000017
/* Parent Signal: CONFIG_GPIO_BTN1 GPIO Pin, (DIO3) */
#define CONFIG_PIN_BTN1                   0x00000003
/* Parent Signal: CONFIG_GPIO_BTN2 GPIO Pin, (DIO4) */
#define CONFIG_PIN_BTN2                   0x00000004
/* Parent Signal: CONFIG_GPIO_RLED GPIO Pin, (DIO6) */
#define CONFIG_PIN_RLED                   0x00000006
/* Parent Signal: CONFIG_GPIO_GLED GPIO Pin, (DIO7) */
#define CONFIG_PIN_GLED                   0x00000007
/* Parent Signal: CONFIG_SPI_0 SCLK, (DIO1) */
#define CONFIG_PIN_1                   0x00000001
/* Parent Signal: CONFIG_SPI_0 MISO, (DIO2) */
#define CONFIG_PIN_2                   0x00000002
/* Parent Signal: CONFIG_SPI_0 MOSI, (DIO0) */
#define CONFIG_PIN_3                   0x00000000
/* Parent Signal: CONFIG_GPIO_0 GPIO Pin, (DIO10) */
#define CONFIG_PIN_0                   0x0000000a
#define CONFIG_TI_DRIVERS_PIN_COUNT    9




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
 *  ======== UART ========
 */

/*
 *  TX: Unassigned
 *  RX: DIO23
 */
extern const uint_least8_t              CONFIG_DISPLAY_UART_CONST;
#define CONFIG_DISPLAY_UART             0
#define CONFIG_TI_DRIVERS_UART_COUNT    1


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
