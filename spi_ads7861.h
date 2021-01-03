#ifndef _SPI_ADS7861_h
#define _SPI_ADS7861_h

int SpiOpenPort (int spi_device);

int SpiClosePort (int spi_device);

int SpiWriteAndRead (int SpiDevice, unsigned char *TxData, unsigned char *RxData, int Length, int LeaveCsLow);

#endif
