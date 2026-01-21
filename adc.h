#ifndef ADC_H
#define ADC_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdint.h>

enum ADC_CHANNELS {
	ADC_CHANNEL_0 = 0b100 << 12,
	ADC_CHANNEL_1 = 0b101 << 12,
	ADC_CHANNEL_2 = 0b110 << 12,
	ADC_CHANNEL_3 = 0b111 << 12
};

#define ADC_SS 0b1 << 15
#define ADC_PGA 0b001 << 9
#define ADC_MODE 0b1 << 8
#define ADC_DR 0b101 << 5
#define ADC_TS 0b0 << 4
#define ADC_PU 0b1 << 3
#define ADC_NOP 0b01 << 1
#define ADC_RES 0b1

union uintToInt {
	uint16_t unsgnd;
	int16_t sgnd;
};

typedef struct ADS_1118 ADS_1118;
struct ADS_1118 {
	SPI_HandleTypeDef *obj;
	GPIO_TypeDef* CSPort;
	uint16_t CSPin;

	void (*init)(ADS_1118*, SPI_HandleTypeDef*, GPIO_TypeDef*, uint16_t);
	double (*read)(ADS_1118*, uint32_t, double);

};

ADS_1118 ADC_new(void);

#endif // ADC_H
