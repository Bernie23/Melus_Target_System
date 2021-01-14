# This is MTS SPI module
import time
import spidev

bus = 0
device = 1

# Enable SPI
spi = spidev.SpiDev()
spi_0 = spi.open(0,0)
spi_1 = spi.open(1,2)

spi.max_speed_hz = 8000000
spi.mode = 0
spi.bits_per_word = 8

def read_adc(vref = 2.5):
    vref = 2.5
    message = [0xC0, 0x00]
    start_message = [0x01]
    spi_1.xfer(start_message)
    reply = spi_0.xfer3(message)
    print(reply)
    
    # Decode the reply
    if reply[0] & 64 == 64:
        channel = 1;
    else:
        channel = 0;
        
    adc_value = '0b' + format(reply[0],'08b')[3:8] + format(reply[1],'08b')[0:7]
    adc_value = int(adc_value, 2)
    voltage = (vref*adc_value)/1024
#     if adc_value >= 2048:
#         voltage = (vref*adc_value)/2048
#     else:
#         voltage = -(vref*adc_value)/2048
    return (channel, voltage)

while True:
    adc_0 = read_adc()
    print("Channel: {}".format(adc_0[0]))
    print("Voltage: {}\n".format(adc_0[1]))
    time.sleep(1)
