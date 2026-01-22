/*
 * @file smv_ads1118.h
 * @brief Provides functionality to connect external adc to microcontroller via SPI
 *
 * Create an instance of struct using pseudo constructor function and
 * call function pointer to collect data
*/

#ifndef _SMV_ADS1118_H
#define _SMV_ADS1118_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* ----- Bitshift Definitions ----- */
#define ADC_SS_SHIFT    15
#define ADC_CHAN_SHIFT	12
#define ADC_PGA_SHIFT   9
#define ADC_MODE_SHIFT  8
#define ADC_DR_SHIFT    5
#define ADC_TS_SHIFT    4
#define ADC_PU_SHIFT    3
#define ADC_NOP_SHIFT   1
#define ADC_RES_SHIFT   0

/* ----- ADC Channels -----
 *
 * SMV in front of define to prevent naming
 * error with HAL adc definitions
 */

/* ----- ADC Settings configuration ----- */
#define ADC_SS      0b1     << ADC_SS_SHIFT     /* width = 1 */

#define ADC_CHANNEL_CLEAR ~(0b111 << ADC_CHAN_SHIFT)
#define ADC_CHANNEL_0	0b100 << ADC_CHAN_SHIFT
#define ADC_CHANNEL_1	0b101 << ADC_CHAN_SHIFT
#define ADC_CHANNEL_2	0b110 << ADC_CHAN_SHIFT
#define ADC_CHANNEL_3	0b111 << ADC_CHAN_SHIFT

#define ADC_PGA     0b001   << ADC_PGA_SHIFT    /* width = 3 */
#define ADC_MODE    0b1     << ADC_MODE_SHIFT   /* width = 1 */
#define ADC_DR      0b101   << ADC_DR_SHIFT     /* width = 3 */
#define ADC_TS      0b0     << ADC_TS_SHIFT     /* width = 1 */
#define ADC_PU      0b1     << ADC_PU_SHIFT     /* width = 1 */
#define ADC_NOP     0b01    << ADC_NOP_SHIFT    /* width = 2 */
#define ADC_RES     0b1     << ADC_RES_SHIFT    /* width = 1 */

/* Factor SPI Calculations */
#define ADC_FACTOR_CALC (4.096)/32767


union uintToInt {
	uint16_t unsgnd;
	int16_t sgnd;
};


/* @brief container to hold current values and settings for each adc instance
 *
 * Order variables from largest->smallest from top->bottom for alignment
*/
typedef struct SMV_ADS1118 SMV_ADS1118;
struct SMV_ADS1118{

	/* data (largest alignment first) */
	uint16_t adc_config;
	SPI_HandleTypeDef * hspi;
	volatile uint8_t error_flag; /* volatile to force consistent checks in memory for flag */

	/* function pointers 4+ bytes */
	double (*read)(SMV_ADS1118*, uint16_t);
	uint8_t (*checkFlag)(SMV_ADS1118*);
	void (*init)(SMV_ADS1118*, SPI_HandleTypeDef *);

};


/* @brief Constructor for external adc instance which includes adc_config setup
 * @param [out] SMV_ads1118 struct object that holds adc values and fault flag
 *
 * defaults to channel 0 unless otherwise specified
 */
SMV_ADS1118 ADS_new(void);

#endif /* _SMV_ADS1118_H */
