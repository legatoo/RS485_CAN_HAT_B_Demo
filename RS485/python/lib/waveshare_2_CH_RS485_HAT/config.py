#!/usr/bin/python
# -*- coding:utf-8 -*-

import serial
import lgpio

TXDEN_1 = 27
TXDEN_2 = 22

# dev = "/dev/ttySC0"

class config(object):
    def __init__(ser, Baudrate = 115200, dev = "/dev/ttyS0"):
        print (dev)
        ser.dev = dev
        ser.serial = serial.Serial(ser.dev, Baudrate)
        try:
            ser.h = lgpio.gpiochip_open(0)
            lgpio.gpio_claim_output(ser.h, TXDEN_1, 1)
            lgpio.gpio_claim_output(ser.h, TXDEN_2, 1)
        except Exception as e:
            print(f"GPIO initialization error: {e}")
            ser.h = None
        
    def Uart_SendByte(ser, value): 
        ser.serial.write(value.encode('ascii')) 
    
    def Uart_SendString(ser, value): 
        ser.serial.write(value.encode('ascii'))

    def Uart_ReceiveByte(ser): 
        return ser.serial.read(1).decode("utf-8")

    def Uart_ReceiveString(ser, value): 
        data = ser.serial.read(value)
        return data.decode("utf-8")
        
    def Uart_Set_Baudrate(ser, Baudrate):
         ser.serial = serial.Serial(ser.dev, Baudrate)
    
    def set_gpio(self, pin, value):
        if self.h is not None:
            lgpio.gpio_write(self.h, pin, value)
    
    def __del__(ser):
        if hasattr(ser, 'h') and ser.h is not None:
            try:
                lgpio.gpiochip_close(ser.h)
            except:
                pass
        
         
         
         