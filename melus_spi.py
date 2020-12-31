# This is MTS SPI module
import time
import spidev

bus = 0
device = 1

# Enable SPI
spi = spidev.SpiDev(0,0)

spi.max_speed_hz = 8000000
spi.mode = 0
spi.bits_per_word = 8

def read_adc(vref = 2.5):
    msg = [0xC0, 0x00, 0xC0, 0x00]
    reply = spi.xfer3(msg)
    
    # Construct a single integer out of the reply
    for number in reply:
        print(bin(number))
    adc = 0
    for n in reply:
        adc = (adc << 8) + n
        
    # Shift to remove the last vaue
    adc = adc >> 1
    
    voltage = (vref*adc)/4096
    
    return voltage


while True:
    adc_0 = read_adc()
    time.sleep(5)
