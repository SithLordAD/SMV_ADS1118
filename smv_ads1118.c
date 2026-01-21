#include "smv_ads1118.h"

static double SMV_ADS1118_Read_Main(SMV_ads1118 *ads){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, (uint16_t*)(ads->adc_config), (uint16_t*)&(ads->spi_buf.unsgnd), 1, 100)!= HAL_OK){
		Error_Handler();
		ads->error_flag = 1;
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	ads->adc_cast = ads->spi_buf.sgnd;
	ads->error_flag = 0;
	return (double)ads->adc_cast * ADC_FACTOR_CALC;
}

static double SMV_ADS1118_Get_ADC(SMV_ads1118 *ads) {
	return ads->adc_value;
}

static uint16_t SMV_ADS1118_Check_Channel(SMV_ads1118 *ads) {
	return ads->adc_channel;
}

static uint8_t SMV_ADS1118_Check_Flag(SMV_ads1118 *ads) {
	return ads->error_flag;
}

/*
 * @brief Pseudo constructor for SMV_ads1118 instance
 * @returns SMV_ads1118 instance
 * 
 * default adc_channel value is Channel 0
*/
SMV_ads1118 ADS_new(void) {
	SMV_ads1118 ads = {
		.adc_value = 0,
		.adc_cast = 0,
		.adc_channel = SMV_ADC_CHANNEL_0,
		.error_flag = 0
	};
	ads.adc_config =
		ADC_SS |
		ads.adc_channel |
		ADC_PGA |
		ADC_MODE |
		ADC_DR |
		ADC_TS |
		ADC_PU |
		ADC_NOP |
		ADC_RES;

	/* function pointer definitions */
	ads.readAdc 		= SMV_ADS1118_Read_Main;
	ads.getAdc 			= SMV_ADS1118_Get_ADC;
	ads.checkChannel 	= SMV_ADS1118_Check_Channel;
	ads.checkFlag 		= SMV_ADS1118_Check_Flag;

	return ads;
}