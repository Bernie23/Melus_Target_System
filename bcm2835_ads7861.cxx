/*
 * spi_bcm2835
 * 
 */


#include <iostream>
#include <ncurses.h>
#include <bcm2835.h>

int kbhit(void);

int main(void)
{
	initscr();
	//Setup SPI pins
	if (!bcm2835_init())
	{
	printf("bcm2835_init failed. Are you running as root??\n");
	return 1;
	}

	if (!bcm2835_spi_begin())
	{
	printf("bcm2835_spi_begin failed. Are you running as root??\n");
	return 1;
	}
	
	//Set bit order
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	
	//Set CS pins polarity to low
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
	
	//Set SPI clock speed
	//< 32 = 128ns = 7.8125MHz
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);

	//Set SPI data mode
	//  	BCM2835_SPI_MODE0 = 0,  // CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	//	BCM2835_SPI_MODE1 = 1,  // CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE2 = 2,  // CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE3 = 3,  // CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);		//(SPI_MODE_# | SPI_CS_HIGH)=Chip Select active high, (SPI_MODE_# | SPI_NO_CS)=1 device per bus no Chip Select

	//Set with CS pin to use for next transfers
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

	char buf[] = {0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00}; // Data to send
	bcm2835_spi_transfern(buf, sizeof(buf));
	printf("Read from SPI:  %02X %02X", buf[0], buf[1]);
	printf("  %02X %02X", buf[2], buf[3]);
	printf("  %02X %02X", buf[4], buf[5]);
	printf("  %02X %02X", buf[6], buf[7]);

	while (!kbhit())
	{
		char buf[] = { 0xC0, 0x00 }; // Data to send
		bcm2835_spi_transfern(buf, sizeof(buf));
		// buf will now be filled with the data that was read from the slave
		//printf("Read from SPI: %02X  %02X \n", buf[0], buf[1]);
		delay(500);
	}

	//Return SPI pins to default inputs state
	bcm2835_spi_end();
	endwin();
}

int kbhit(void)
{
    int ch, r;
       
    /* Turn off getch() blocking and echo */
    nodelay(stdscr, TRUE);
    noecho();
       
    /* Check for input */
    ch = getch();
    if (ch == ERR)  /* no input */
        r = FALSE;
    else            /* input, push it back */
    {
        r = TRUE;
        ungetch(ch);
    }
       
    /* Restore blocking and echo */
    echo();
    nodelay(stdscr, FALSE);
    return r;
}

void bcm2835_aux_spi_transfernb(const char *tbuf, char *rbuf, uint32_t len) {
    volatile uint32_t* cntl0 = bcm2835_spi1 + BCM2835_AUX_SPI_CNTL0/4;
    volatile uint32_t* cntl1 = bcm2835_spi1 + BCM2835_AUX_SPI_CNTL1/4;
    volatile uint32_t* stat = bcm2835_spi1 + BCM2835_AUX_SPI_STAT/4;
    volatile uint32_t* txhold = bcm2835_spi1 + BCM2835_AUX_SPI_TXHOLD/4;
    volatile uint32_t* io = bcm2835_spi1 + BCM2835_AUX_SPI_IO/4;

	char *tx = (char *)tbuf;
	char *rx = (char *)rbuf;
	uint32_t tx_len = len;
	uint32_t rx_len = len;
	uint32_t count;
	uint32_t data;
	uint32_t i;
	uint8_t byte;

	uint32_t _cntl0 = (spi1_speed << BCM2835_AUX_SPI_CNTL0_SPEED_SHIFT);
	_cntl0 |= BCM2835_AUX_SPI_CNTL0_CS2_N;
	_cntl0 |= BCM2835_AUX_SPI_CNTL0_ENABLE;
	_cntl0 |= BCM2835_AUX_SPI_CNTL0_MSBF_OUT;
	_cntl0 |= BCM2835_AUX_SPI_CNTL0_VAR_WIDTH;

	bcm2835_peri_write(cntl0, _cntl0);
	bcm2835_peri_write(cntl1, BCM2835_AUX_SPI_CNTL1_MSBF_IN);

	while ((tx_len > 0) || (rx_len > 0)) {

		while (!(bcm2835_peri_read(stat) & BCM2835_AUX_SPI_STAT_TX_FULL) && (tx_len > 0)) {
			count = MIN(tx_len, 3);
			data = 0;

			for (i = 0; i < count; i++) {
				byte = (tx != NULL) ? (uint8_t) *tx++ : (uint8_t) 0;
				data |= byte << (8 * (2 - i));
			}

			data |= (count * 8) << 24;
			tx_len -= count;

			if (tx_len != 0) {
				bcm2835_peri_write(txhold, data);
			} else {
				bcm2835_peri_write(io, data);
			}

		}

		while (!(bcm2835_peri_read(stat) & BCM2835_AUX_SPI_STAT_RX_EMPTY) && (rx_len > 0)) {
			count = MIN(rx_len, 3);
			data = bcm2835_peri_read(io);

			if (rbuf != NULL) {
				switch (count) {
				case 3:
					*rx++ = (char)((data >> 16) & 0xFF);
					/*@fallthrough@*/
					/* no break */
				case 2:
					*rx++ = (char)((data >> 8) & 0xFF);
					/*@fallthrough@*/
					/* no break */
				case 1:
					*rx++ = (char)((data >> 0) & 0xFF);
				}
			}

			rx_len -= count;
		}

		while (!(bcm2835_peri_read(stat) & BCM2835_AUX_SPI_STAT_BUSY) && (rx_len > 0)) {
			count = MIN(rx_len, 3);
			data = bcm2835_peri_read(io);

			if (rbuf != NULL) {
				switch (count) {
				case 3:
					*rx++ = (char)((data >> 16) & 0xFF);
					/*@fallthrough@*/
					/* no break */
				case 2:
					*rx++ = (char)((data >> 8) & 0xFF);
					/*@fallthrough@*/
					/* no break */
				case 1:
					*rx++ = (char)((data >> 0) & 0xFF);
				}
			}

			rx_len -= count;
		}
	}
}
