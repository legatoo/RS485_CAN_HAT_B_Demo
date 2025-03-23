#!/usr/bin/python
# -*- coding:utf-8 -*-
import serial
import time

import os
import sys
import logging

logging.basicConfig(level=logging.INFO)
libdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'lib')
if os.path.exists(libdir):
    sys.path.append(libdir)

from waveshare_2_CH_RS485_HAT import config

#TXDEN_1 = 27
#TXDEN_2 = 22

ser1 = config.config(dev = "/dev/ttySC0")
ser2 = config.config(dev = "/dev/ttySC1")
data1 = 'waveshare_2_CH_RS485_HAT_1to2\r\n'
data2 = 'waveshare_2_CH_RS485_HAT_2to1\r\n'
# data = data.decode("bytes")
while(1):
    #lgpio.gpio_write(h, TXDEN_1, 0) #send
    #lgpio.gpio_write(h, TXDEN_2, 1) #send
    ser1.Uart_SendString(data1)
    data_t = ser2.Uart_ReceiveString(31)
    if(data_t == data1):
        print ('Channel 1 send channel 2 received successfully')
        print (data_t)
        data_t = ''
    
    #lgpio.gpio_write(h, TXDEN_1, 1) #send
    #lgpio.gpio_write(h, TXDEN_2, 0) #send
    ser2.Uart_SendString(data2)
    data_t = ser1.Uart_ReceiveString(31)
    if(data_t == data2):
        print ('Channel 2 send channel 1 received successfully')
        print (data_t)
        data_t = ''
    print('\r\n')
    time.sleep(1)

     
     
     
     
