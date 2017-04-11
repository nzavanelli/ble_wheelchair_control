//wheelchair_control_spi.h

#include "app_util_platform.h"
//SPI PINS
#define WHEELCHAIR_CONTROL_SPI_CS1 9
#define WHEELCHAIR_CONTROL_SPI_CS2 10
#define WHEELCHAIR_CONTROL_SPI_MOSI 8
#define WHEELCHAIR_CONTROL_SPI_SCLK 11
//LED Pins
#define LED0 14
#define LED1 13
#define LED2 15
#define LED3 16


void spi_init(void);

uint8_t wheelchair_spi_write(uint8_t dac_pin, uint8_t byte1, uint8_t byte2);

uint8_t encodeFirstByte(uint8_t byte1);

uint8_t highbyte(uint16_t val);

uint8_t lowbyte(uint16_t val);

uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac);

bool wheelchair_reset_init(uint16_t *dac1_value, uint16_t *dac2_value);

bool wheelchair_reset(bool is_reset, uint16_t *dac1_value, uint16_t *dac2_value);

void wheelchair_move_forward(uint16_t *dac1_value, uint16_t *dac2_value);

void wheelchair_move_left(uint16_t *dac1_value, uint16_t *dac2_value);

void wheelchair_move_right(uint16_t *dac1_value, uint16_t *dac2_value);

void wheelchair_move_reverse(uint16_t *dac1_value, uint16_t *dac2_value);

