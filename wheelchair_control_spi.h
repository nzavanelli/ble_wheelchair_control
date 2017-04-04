//wheelchair_control_spi.h
#define WHEELCHAIR_CONTROL_SPI_CS1 15
#define WHEELCHAIR_CONTROL_SPI_CS2 16

#define WHEELCHAIR_CONTROL_SPI_MOSI 9
#define WHEELCHAIR_CONTROL_SPI_SCLK 10
void spi_init(void);

void wheelchair_reset(void);

void wheelchair_move_forward(void);

void wheelchair_move_left(void);

void wheelchair_move_right(void);

