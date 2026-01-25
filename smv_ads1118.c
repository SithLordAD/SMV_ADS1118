#include "smv_ads1118.h"
#include <stdint.h>

void Error_Handler(void); // Must be provided by the user

/* union to convert b/w unsigned and signed */
typedef union {
    uint16_t u;
    int16_t  s;
} ADS1118_Caster;


static double ADS1118_GetLSB(ADS1118 *adc);

/**
 * Purpose: a function to quickly setup our SPI settings, uses some defaults for the bitfield
 * - allows for use across multiple SPI clocks
 *
 * Enables SPI for 16 bit data, is abstracted enough to reduce need IOC configurations
 */
static void ADS1118_QuickSetup(ADS1118 *adc, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin){
    adc->hspi = hspi;
    adc->hspi->Instance = SPI1;


    if (adc->hspi->Instance == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    else if (adc->hspi->Instance == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    else if (adc->hspi->Instance == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();

    /* Configure SPI settings */
    adc->hspi->Init.Mode              = SPI_MODE_MASTER;
    adc->hspi->Init.Direction         = SPI_DIRECTION_2LINES;
    adc->hspi->Init.DataSize          = SPI_DATASIZE_16BIT;
    adc->hspi->Init.CLKPolarity       = SPI_POLARITY_LOW;
    adc->hspi->Init.CLKPhase          = SPI_PHASE_1EDGE;
    adc->hspi->Init.NSS               = SPI_NSS_SOFT;
    adc->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    adc->hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
    adc->hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
    adc->hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    adc->hspi->Init.CRCPolynomial     = 10;

    if (HAL_SPI_Init(adc->hspi) != HAL_OK) Error_Handler();

    /* Init chip select info */
    adc->cs_port  = cs_port;
    adc->cs_pin   = cs_pin;

    /* Set default parameters in the bit field */
    adc->last_raw   = 0;
    adc->config.inputCode = 0;      // clear first
    adc->config.bits.reserved = 1;  // REQUIRED by datasheet
    adc->config.bits.nop      = 0b01;
    adc->config.bits.pullup  = 1;
    adc->config.bits.dr      = 0b101;
    adc->config.bits.mode    = 1;
    adc->config.bits.start   = 1;
    adc->config.bits.pga     = ADS1118_PGA_4_096V;
    adc->lsb_factor = ADS1118_GetLSB(adc); // generates the factor to normalize the ratio
}

/*
 *  Read the raw ADC reading on the given channel
 */
static int16_t ADS1118_ReadRaw(ADS1118 *adc, ADC_CHANNELS channel)
{
    /* Set MUX and leave rest of config intact */
    adc->config.bits.mux = channel;

    uint16_t inputCode = adc->config.inputCode;
    ADS1118_Caster recievedData;

    /* Send input code recieve reading */
    HAL_GPIO_WritePin(adc->cs_port, adc->cs_pin, GPIO_PIN_RESET);

    if (HAL_SPI_TransmitReceive(adc->hspi, (uint16_t*)&inputCode, (uint16_t*)&recievedData.u, 1, 100) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_GPIO_WritePin(adc->cs_port, adc->cs_pin, GPIO_PIN_SET);

    adc->last_raw = recievedData.s;
    return recievedData.s;
}

/*
 * Convert raw reading to normalized value
 */
static double ADS1118_Read(ADS1118 *adc, ADC_CHANNELS channel)
{
    int16_t raw = ADS1118_ReadRaw(adc, channel);
    return (double)raw * adc->lsb_factor;
}

/* ===============================
   Helper: set PGA and update LSB factor
   =============================== */
void ADS1118_SetPGA(ADS1118 *adc, uint8_t pga_bits, double fsr)
{
    adc->config.bits.pga = pga_bits;
    adc->lsb_factor = (fsr * 2.0) / 32767.0;
}

/* ===============================
   Constructor
   =============================== */
ADS1118 ADS1118_new(void)
{
    ADS1118 adc;

    adc.init    = ADS1118_QuickSetup;
    adc.read    = ADS1118_Read;
    adc.readRaw = ADS1118_ReadRaw;

    return adc;
}

/* Helper function to use PGA FSR value for the factor */
static double ADS1118_GetLSB(ADS1118 *adc)
{
    double fsr = 0.0;

    switch (adc->config.bits.pga)
    {
        case 0b000: fsr = 6.144; break;
        case 0b001: fsr = 4.096; break;
        case 0b010: fsr = 2.048; break;
        case 0b011: fsr = 1.024; break;
        case 0b100: fsr = 0.512; break;
        case 0b101:
        case 0b110:
        case 0b111: fsr = 0.256; break;
    }

    return (fsr * 2.0) / 32767.0;
}

