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
 *  DO NOT EDIT - This file is generated for the CC26X2R1_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC26X2R1_LAUNCHXL
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
 *  ======== ADC ========
 */

/* DIO23 */
extern const uint_least8_t              CONFIG_ADC_0_CONST;
#define CONFIG_ADC_0                    0
#define CONFIG_TI_DRIVERS_ADC_COUNT     1


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

/* DIO13, LaunchPad Button BTN-1 (Left) */
extern const uint_least8_t              CONFIG_GPIO_BTN1_CONST;
#define CONFIG_GPIO_BTN1                0
/* DIO14, LaunchPad Button BTN-2 (Right) */
extern const uint_least8_t              CONFIG_GPIO_BTN2_CONST;
#define CONFIG_GPIO_BTN2                1
/* DIO6, LaunchPad LED Red */
extern const uint_least8_t              CONFIG_GPIO_RLED_CONST;
#define CONFIG_GPIO_RLED                2
/* DIO7, LaunchPad LED Green */
extern const uint_least8_t              CONFIG_GPIO_GLED_CONST;
#define CONFIG_GPIO_GLED                3
/* DIO11 */
extern const uint_least8_t              CONFIG_GPIO_11_CS_CONST;
#define CONFIG_GPIO_11_CS               4
/* DIO22 */
extern const uint_least8_t              CONFIG_GPIO22_PWDN_CONST;
#define CONFIG_GPIO22_PWDN              5
/* DIO21 */
extern const uint_least8_t              CONFIG_GPIO_21_1298RST_CONST;
#define CONFIG_GPIO_21_1298RST          6
/* DIO15 */
extern const uint_least8_t              CONFIG_GPIO_15adssrt_CONST;
#define CONFIG_GPIO_15adssrt            7
/* DIO12 */
extern const uint_least8_t              CONFIG_GPI012_drdy_CONST;
#define CONFIG_GPI012_drdy              8
/* DIO20, MX25R8035F SPI Flash Slave Select */
extern const uint_least8_t              CONFIG_GPIO_0_CONST;
#define CONFIG_GPIO_0                   9
#define CONFIG_TI_DRIVERS_GPIO_COUNT    10

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
 *  MOSI: DIO9
 *  MISO: DIO8
 *  SCLK: DIO10
 *  LaunchPad SPI Bus
 *  SS: DIO20, MX25R8035F SPI Flash Slave Select
 */
extern const uint_least8_t              CONFIG_NVSEXTERNAL_CONST;
#define CONFIG_NVSEXTERNAL              1
#define CONFIG_TI_DRIVERS_NVS_COUNT     2


/*
 *  ======== PIN ========
 */
#include <ti/drivers/PIN.h>

extern const PIN_Config BoardGpioInitTable[];

/* Parent Signal: CONFIG_ADC_0 ADC Pin, (DIO23) */
#define CONFIG_PIN_9                   0x00000017
/* LaunchPad Button BTN-1 (Left), Parent Signal: CONFIG_GPIO_BTN1 GPIO Pin, (DIO13) */
#define CONFIG_PIN_BTN1                   0x0000000d
/* LaunchPad Button BTN-2 (Right), Parent Signal: CONFIG_GPIO_BTN2 GPIO Pin, (DIO14) */
#define CONFIG_PIN_BTN2                   0x0000000e
/* LaunchPad LED Red, Parent Signal: CONFIG_GPIO_RLED GPIO Pin, (DIO6) */
#define CONFIG_PIN_RLED                   0x00000006
/* LaunchPad LED Green, Parent Signal: CONFIG_GPIO_GLED GPIO Pin, (DIO7) */
#define CONFIG_PIN_GLED                   0x00000007
/* Parent Signal: CONFIG_GPIO_11_CS GPIO Pin, (DIO11) */
#define CONFIG_PIN_4                   0x0000000b
/* Parent Signal: CONFIG_GPIO22_PWDN GPIO Pin, (DIO22) */
#define CONFIG_PIN_5                   0x00000016
/* Parent Signal: CONFIG_GPIO_21_1298RST GPIO Pin, (DIO21) */
#define CONFIG_PIN_6                   0x00000015
/* Parent Signal: CONFIG_GPIO_15adssrt GPIO Pin, (DIO15) */
#define CONFIG_PIN_7                   0x0000000f
/* Parent Signal: CONFIG_GPI012_drdy GPIO Pin, (DIO12) */
#define CONFIG_PIN_8                   0x0000000c
/* LaunchPad SPI Bus, Parent Signal: CONFIG_SPI_0 SCLK, (DIO10) */
#define CONFIG_PIN_1                   0x0000000a
/* LaunchPad SPI Bus, Parent Signal: CONFIG_SPI_0 MISO, (DIO8) */
#define CONFIG_PIN_2                   0x00000008
/* LaunchPad SPI Bus, Parent Signal: CONFIG_SPI_0 MOSI, (DIO9) */
#define CONFIG_PIN_3                   0x00000009
/* Parent Signal: CONFIG_UART_0 TX, (DIO1) */
#define CONFIG_PIN_10                   0x00000001
/* Parent Signal: CONFIG_UART_0 RX, (DIO0) */
#define CONFIG_PIN_11                   0x00000000
/* XDS110 UART, Parent Signal: CONFIG_DISPLAY_UART TX, (DIO3) */
#define CONFIG_PIN_UART_TX                   0x00000003
/* XDS110 UART, Parent Signal: CONFIG_DISPLAY_UART RX, (DIO2) */
#define CONFIG_PIN_UART_RX                   0x00000002
/* MX25R8035F SPI Flash Slave Select, Parent Signal: CONFIG_GPIO_0 GPIO Pin, (DIO20) */
#define CONFIG_PIN_0                   0x00000014
#define CONFIG_TI_DRIVERS_PIN_COUNT    18




/*
 *  ======== SPI ========
 */

/*
 *  MOSI: DIO9
 *  MISO: DIO8
 *  SCLK: DIO10
 *  LaunchPad SPI Bus
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
 *  TX: DIO1
 *  RX: DIO0
 */
extern const uint_least8_t              CONFIG_UART_0_CONST;
#define CONFIG_UART_0                   0
/*
 *  TX: DIO3
 *  RX: DIO2
 *  XDS110 UART
 */
extern const uint_least8_t              CONFIG_DISPLAY_UART_CONST;
#define CONFIG_DISPLAY_UART             1
#define CONFIG_TI_DRIVERS_UART_COUNT    2


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
