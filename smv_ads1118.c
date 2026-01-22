#include "smv_ads1118.h"

void Error_Handler(void); // must be provided by user

static double SMV_ADS1118_Read(SMV_ADS1118 *ads, uint16_t adc_channel){
	int16_t adc_cast = 0;
	union uintToInt spi_buf;

//	ads->adc_config = ((ads->adc_config) & ADC_CHANNEL_CLEAR) | adc_channel;
	if (adc_channel == ADC_CHANNEL_0){
		ads->adc_config = 0b1100001110101011;
	}else if (adc_channel == ADC_CHANNEL_1){
		ads->adc_config = 0b1101001110101011;
	}else if (adc_channel == ADC_CHANNEL_2){
		ads->adc_config = 0b1110001110101011;
	}else if (adc_channel == ADC_CHANNEL_3){
		ads->adc_config = 0b1111001110101011;
	}else{
		return -1;
	}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(ads->hspi, (uint16_t*)&(ads->adc_config), (uint16_t*)&(spi_buf.unsgnd), 1, 100)!= HAL_OK){
		ads->error_flag = 1;
		Error_Handler();
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	adc_cast = spi_buf.sgnd;
	ads->error_flag = 0;
	return (double)adc_cast * ADC_FACTOR_CALC;
}

static uint8_t SMV_ADS1118_Check_Flag(SMV_ADS1118 *ads) {
	return ads->error_flag;
}

static void SMV_ADS1118_Setup (SMV_ADS1118 *ads, SPI_HandleTypeDef * hspi_pass){
	ads->hspi = hspi_pass;
	ads->hspi->Instance = SPI1;
	ads->hspi->Init.Mode = SPI_MODE_MASTER;
	ads->hspi->Init.Direction = SPI_DIRECTION_2LINES;
	ads->hspi->Init.DataSize = SPI_DATASIZE_16BIT;
	ads->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	ads->hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
	ads->hspi->Init.NSS = SPI_NSS_SOFT;
	ads->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	ads->hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	ads->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	ads->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	ads->hspi->Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(ads->hspi) != HAL_OK)
	{
		ads->error_flag = 1;
		Error_Handler();
	}
}

/*
 * @brief Pseudo constructor for SMV_ads1118 instance
 * @returns SMV_ads1118 instance
 *
 * default adc_channel value is Channel 0
*/
SMV_ADS1118 ADS_new(void) {
	SMV_ADS1118 ads = {
		.error_flag = 0
	};
	ads.adc_config =
		ADC_SS |
		ADC_CHANNEL_CLEAR |
		ADC_PGA |
		ADC_MODE |
		ADC_DR |
		ADC_TS |
		ADC_PU |
		ADC_NOP |
		ADC_RES;

	/* function pointer definitions */
	ads.read	 		= SMV_ADS1118_Read;
	ads.checkFlag 		= SMV_ADS1118_Check_Flag;
	ads.init 			= SMV_ADS1118_Setup;

	return ads;
}
