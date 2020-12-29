# This is MTS SPI module
import time
import spidev

bus = 0
device = 1

# Enable SPI
spi = spidev.SpiDev(0,0)

spi.max_speed_hz = 8000000
spi.mode = 0

def read_adc(adc_ch, vref = 3.3):
    msg = 0b11
    msg = ((msg << 1) + adc_ch) << 5
    msg = [msg, 0b00000000]
    reply = spi.xfer2(msg)
    
    # Construct a single integer out of the reply
    adc = 0
    for n in reply:
        adc = (adc << 8) + n
        
    # Shift to remove the last vaue
    adc = adc >> 1
    
    voltage = (vref*adc)/1024
    
    return voltage


while True:
    adc_0 = read_adc(0)
    adc_1 = read_adc(1)
