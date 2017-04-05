//wheelchair_control_spi.c
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "wheelchair_control_spi.h"
/**@headers for µs delay:*/
#include <stdio.h> 
#include "compiler_abstraction.h"
#include "nrf.h"
#include "nrf_gpio.h"

void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
		/*switch (p_event->type) {
				case NRF_DRV_SPI_EVENT_DONE:
					break;
				default:
					break;
		}*/
    //NRF_LOG_PRINTF(" >>> Transfer completed.\r\n");
}

/**@INITIALIZE SPI INSTANCE */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0); //SPI INSTANCE


void spi_init(void) {
		uint8_t errcode;
		nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG(0);
		spi_config.bit_order						= NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
		spi_config.frequency						= NRF_DRV_SPI_FREQ_1M;
		spi_config.irq_priority					= APP_IRQ_PRIORITY_LOW;
		spi_config.mode									= NRF_DRV_SPI_MODE_0; //CPOL = 0 (Active High); CPHA = 0
		spi_config.miso_pin 						= NRF_DRV_SPI_PIN_NOT_USED;
		spi_config.sck_pin 							= WHEELCHAIR_CONTROL_SPI_SCLK;
		spi_config.mosi_pin 						= WHEELCHAIR_CONTROL_SPI_MOSI;
		spi_config.ss_pin								= NRF_DRV_SPI_PIN_NOT_USED;
		spi_config.orc									= 0x55;
		errcode = nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);
		NRF_LOG_PRINTF(" SPI Init Errcode = [%d] \r\n",errcode);
		APP_ERROR_CHECK(errcode);
}

uint8_t wheelchair_spi_write(uint8_t dac, uint8_t byte1, uint8_t byte2) {
	uint8_t errcode;
	uint8_t cs_dac_pin = 0;
	if (dac == 1) {
		cs_dac_pin = WHEELCHAIR_CONTROL_SPI_CS1;
	} else {
		cs_dac_pin = WHEELCHAIR_CONTROL_SPI_CS2;
	}
	//TODO: MODIFY HIGHBYTE (BYTE1)?
	uint8_t tx_transfer_data[2] = {byte1, byte2};
	nrf_gpio_pin_clear(cs_dac_pin); //PULL CS LOW
	errcode = nrf_drv_spi_transfer(&spi, tx_transfer_data, 2, NULL, 0);
	//delay?
	nrf_delay_ms(1);
	nrf_gpio_pin_set(cs_dac_pin);		//PULL CS BACK HIGH
	return errcode;
}

uint8_t encodeFirstByte(uint8_t byte1) {
	uint8_t retval = 0x0F & byte1;
	return (0x30 | retval);
}

uint8_t highbyte(uint16_t val) {
	return (val >> 8);
}

uint8_t lowbyte(uint16_t val) {
	return (val & 0xFF);
}
//using iterative method: ++
//uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac)
	
uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac) {
		NRF_LOG_PRINTF("Entered Changespeed \r\n");	
		uint8_t errcode = 0;
		uint16_t diff; //Voltage difference in int. 
		uint16_t RoC;
		uint16_t i = 0; 
		uint8_t	stp = 100;
		uint16_t outputValue = 0;
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		if (*currentHex > desiredState) {
			NRF_LOG_PRINTF("Vals b4 while loop: cH:[%d]>dS:[%d] \r\n",*currentHex, desiredState);
			diff = *currentHex-desiredState;
			RoC = diff/stp;
			NRF_LOG_PRINTF("Vals b4 while loop: [%d] [%d] \r\n",diff, RoC);
			while (i<stp) {
				*currentHex -= RoC;
				outputValue = *currentHex;
				byte1 = highbyte(outputValue);
				byte2 = lowbyte(outputValue);
				byte1 = encodeFirstByte(byte1);
				errcode = wheelchair_spi_write(dac, byte1, byte2);
				NRF_LOG_PRINTF("Vals IN while loop:i=%d [%d] \r\n",i,*currentHex);
				i++;
			}
		} 
		
		if (desiredState > *currentHex) {
			NRF_LOG_PRINTF("Vals b4 while loop: cH:[%d]<dS:[%d] \r\n",*currentHex, desiredState);
			diff = desiredState - *currentHex;
			RoC = (diff/stp);
			NRF_LOG_PRINTF("Vals b4 while loop: [%d] [%d] \r\n",diff, RoC);
			while(i<stp) {
				*currentHex += RoC;
				outputValue = *currentHex;
				byte1 = highbyte(outputValue);
				byte2 = lowbyte(outputValue);
				byte1 = encodeFirstByte(byte1);
				errcode = wheelchair_spi_write(dac,byte1,byte2);
				NRF_LOG_PRINTF("Vals IN while loop:i=%d [%d] \r\n",i,*currentHex);
				i++;
			}
		}
		//because int values are only an estimation, jump the last value:
		*currentHex = desiredState;
		outputValue = *currentHex;
		byte1 = highbyte(outputValue);
			byte2 = lowbyte(outputValue);
			byte1 = encodeFirstByte(byte1);
			errcode = wheelchair_spi_write(dac,byte1,byte2);
			NRF_LOG_PRINTF("Final Vals:i=%d [%d] \r\n",i,*currentHex);
		return errcode;
}

bool wheelchair_reset(bool is_reset, uint16_t *dac1_value, uint16_t *dac2_value) {
	uint8_t errcode;
	uint16_t desiredState = 2083;//2.50*833.33 = 2083.325 
	//adjusted due to poor-tolerance resistors.  // 2053
		//Value is same for both DACs
	if(!is_reset) {
		//if not yet reset:
		//Start with DAC 2:
		NRF_LOG_PRINTF("RESETTING DAC 2\r\n");
		errcode = changespeed(dac2_value, desiredState, 2);
		//Then DAC 1:
		NRF_LOG_PRINTF("RESETTING DAC 1\r\n");
		errcode = changespeed(dac1_value, desiredState, 1);
	}
	NRF_LOG_PRINTF("wheelchair_reset errcode = [%d] \r\n",errcode);
	return true;
}

void wheelchair_move_forward(uint16_t *dac1_value, uint16_t *dac2_value) {
		uint8_t errcode;
	//DAC 2:
		uint16_t desiredState = 1967; //2.36*833.33 = 1966.6588
		errcode = changespeed(dac2_value, desiredState, 2); //Pass by reference. 
		NRF_LOG_PRINTF("WHC_MV_FWD: DAC2 ERRCODE [%d]\r\n",errcode);
	//DAC 1:
		uint16_t desiredState2 = 2250; //2.70*833.33 = 2249.991
		errcode = changespeed(dac1_value, desiredState2, 1);
		NRF_LOG_PRINTF("WHC_MV_FWD: DAC1 ERRCODE [%d]\r\n",errcode);
}

void wheelchair_move_left(uint16_t *dac1_value, uint16_t *dac2_value) {
		uint8_t errcode;
	//DAC 2:
		uint16_t desiredState = 2250; //2.70*833.33 = 2249.991
		errcode = changespeed(dac2_value, desiredState, 2); //Pass by reference. 
		NRF_LOG_PRINTF("WHC_MV_LEFT: DAC2 ERRCODE [%d]\r\n",errcode);
	//DAC 1:
		uint16_t desiredState2 = 2083; //2.50*833.33 = 2083.325
		errcode = changespeed(dac1_value, desiredState2, 1);
		NRF_LOG_PRINTF("WHC_MV_LEFT: DAC1 ERRCODE [%d]\r\n",errcode);
}

void wheelchair_move_right(uint16_t *dac1_value, uint16_t *dac2_value) {
		uint8_t errcode;
	//DAC 2:
		uint16_t desiredState = 1708; //2.05*833.33 = 1708.3265
		errcode = changespeed(dac2_value, desiredState, 2); //Pass by reference. 
		NRF_LOG_PRINTF("WHC_MV_RIGHT: DAC2 ERRCODE [%d]\r\n",errcode);
	//DAC 1:
		uint16_t desiredState2 = 2083; //2.50*833.33 = 2083.325
		errcode = changespeed(dac1_value, desiredState2, 1);
		NRF_LOG_PRINTF("WHC_MV_RIGHT: DAC1 ERRCODE [%d]\r\n",errcode);
}

void wheelchair_move_reverse(uint16_t *dac1_value, uint16_t *dac2_value) {
		NRF_LOG_PRINTF("REVERSE NOT YET IMPLEMENTED! [%d]\r\n");
}
