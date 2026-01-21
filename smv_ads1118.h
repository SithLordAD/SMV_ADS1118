#ifndef __SMVADS1118_H
#define __SMVADS1118_H

#include "stm32f4xx_hal.h"


/*
 * Input MUX selection
 */
typedef enum {
    ADC_CHANNEL_0 = 0b100,
    ADC_CHANNEL_1 = 0b101,
    ADC_CHANNEL_2 = 0b110,
    ADC_CHANNEL_3 = 0b111
} ADC_CHANNELS;


/*
 * Programmable Gain Amplifier (FSR)
 */
typedef enum {
    ADS1118_PGA_6_144V = 0b000,
    ADS1118_PGA_4_096V = 0b001,
    ADS1118_PGA_2_048V = 0b010,
    ADS1118_PGA_1_024V = 0b011,
    ADS1118_PGA_0_512V = 0b100,
    ADS1118_PGA_0_256V = 0b101
} ADS1118_PGA;


/*
 * 16-bit field for input code to receive data from the ADC
 */
typedef struct {
    uint16_t reserved : 1;  /* Bit 0  - Reserved (must be 1) */
    uint16_t nop      : 2;  /* Bits 1–2 - No operation */
    uint16_t pullup   : 1;  /* Bit 3  - DOUT pull-up enable */
    uint16_t ts_mode  : 1;  /* Bit 4  - Temperature sensor enable */
    uint16_t dr       : 3;  /* Bits 5–7 - Data rate */
    uint16_t mode     : 1;  /* Bit 8  - Operating mode */
    uint16_t pga      : 3;  /* Bits 9–11 - PGA setting */
    uint16_t mux      : 3;  /* Bits 12–14 - Input MUX */
    uint16_t start    : 1;  /* Bit 15 - Start conversion */
} ADS1118_ConfigBits;

typedef union {
    ADS1118_ConfigBits bits;
    uint16_t           inputCode;
} ADS1118_Config;


typedef struct ADS1118 ADS1118;
struct ADS1118 {
    SPI_HandleTypeDef *hspi; /* Pointer to SPI handle */
    GPIO_TypeDef *cs_port; /* Pointer to the GPIO port for the chip select pin*/
    uint16_t cs_pin; /* The actual GPIO pin number, i.e GPIO_PIN_4

     /*
        16-bit input code
        Uses a bit field union (MSB)
        Contains:
          - Start bit [15]
          - Input multiplexer selection [14:12]
          - PGA (full-scale range) [11:0]
          - Operating mode [8]
          - Data sampling rate [7:5]
          - Temperature sensor enable (default to 0) [4]
          - Pull-up enable [3]
          - No-operation bits [2:1]
          - Reserved bit [0]
    */
    ADS1118_Config config;
    double lsb_factor; /* Conversion factor to convert from ratio to actual voltage */
    int16_t last_raw; /* Raw data ADC reading received over SPI */

    /* Methods */
    void   (*init)(ADS1118*, SPI_HandleTypeDef*, GPIO_TypeDef*, uint16_t);
    double (*read)(ADS1118*, ADC_CHANNELS);
    int16_t (*readRaw)(ADS1118*, ADC_CHANNELS);
};

/*
 * Constructor workaround, call when using this object
 */
ADS1118 ADS1118_new(void);

#endif /* __SMVADS1118_H */
