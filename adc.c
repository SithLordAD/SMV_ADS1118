#include "adc.h"

void Error_Handler(void);

static void SPI_Setup(ADS_1118 *adc, SPI_HandleTypeDef *adcObject, GPIO_TypeDef* CSPort, uint16_t CSPin) {
	adc->obj = adcObject;
	adc->CSPort = CSPort;
	adc->CSPin = CSPin;

	adc->obj->Instance = SPI1;
	adc->obj->Init.Mode = SPI_MODE_MASTER;
	adc->obj->Init.Direction = SPI_DIRECTION_2LINES;
	adc->obj->Init.DataSize = SPI_DATASIZE_16BIT;
	adc->obj->Init.CLKPolarity = SPI_POLARITY_LOW;
	adc->obj->Init.CLKPhase = SPI_PHASE_2EDGE;
	adc->obj->Init.NSS = SPI_NSS_SOFT;
	adc->obj->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	adc->obj->Init.FirstBit = SPI_FIRSTBIT_MSB;
	adc->obj->Init.TIMode = SPI_TIMODE_DISABLE;
	adc->obj->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	adc->obj->Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(adcObject) != HAL_OK) {
		Error_Handler();
	}
}

static double readData(ADS_1118 *adc, uint32_t ADC_Channel, double factor) {
	int16_t adc_cast = 0;
	union uintToInt spi_buf;
	int16_t input_code =
	ADC_SS |
	ADC_Channel |
	ADC_PGA |
	ADC_MODE |
	ADC_DR |
	ADC_TS |
	ADC_PU |
	ADC_NOP |
	ADC_RES;

	HAL_GPIO_WritePin(adc->CSPort, adc->CSPin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(adc->obj, (uint8_t*) (&input_code),
			(uint8_t*) &(spi_buf.unsgnd), 1, 100) != HAL_OK) {
		Error_Handler();
	}
	HAL_GPIO_WritePin(adc->CSPort, adc->CSPin, GPIO_PIN_SET);

	adc_cast = spi_buf.sgnd;
	return (double) adc_cast * factor;
}

ADS_1118 ADC_new(void) {
	ADS_1118 adc_obj;
	adc_obj.init = SPI_Setup;
	adc_obj.read = readData;
	return adc_obj;
}
