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
		APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler));
		NRF_LOG_PRINTF(" SPI Initialized..\r\n");
		//nrf_gpio_pin_clear(CS);
}

uint16_t changespeed(uint16_t currentHex, uint16_t desiredState, uint8_t dac) {
		uint16_t ret_val;
		double diff;
		double RoC;
		int stp = 100;
		uint16_t i = 0; 
		NRF_LOG_PRINTF("Entered Changespeed \r\n");
		if (currentHex > desiredState) {
			diff = currentHex-desiredState;
			RoC = diff/stp;
			while (i<stp) {
				
			}
		}
		return 0;
}

void wheelchair_reset(void) {
	//Bring DAC 1 Low:
	//NEUTRAL HEX VALUE:
	//uint16_t output_val = 0x0823;
	uint8_t tx_data_spi[2] = {0x08, 0x23};
	tx_data_spi[0] = 0x0F & tx_data_spi[0];
	tx_data_spi[0] = 0x30 | tx_data_spi[0];
	nrf_gpio_pin_clear(WHEELCHAIR_CONTROL_SPI_CS1);
	nrf_drv_spi_transfer(&spi, tx_data_spi, 2, NULL, 0);
	nrf_gpio_pin_set(WHEELCHAIR_CONTROL_SPI_CS1);
	
	nrf_gpio_pin_clear(WHEELCHAIR_CONTROL_SPI_CS2);
	nrf_drv_spi_transfer(&spi, tx_data_spi, 2, NULL, 0);
	nrf_gpio_pin_set(WHEELCHAIR_CONTROL_SPI_CS2);
}

void wheelchair_move_forward(void) {
	
}

