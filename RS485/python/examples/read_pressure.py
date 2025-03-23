#!/usr/bin/python
# -*- coding:utf-8 -*-
import serial
import time
import struct
import logging

logging.basicConfig(level=logging.INFO)

class ModbusRTU:
    def __init__(self, port="/dev/ttySC0", baudrate=9600, slave_id=1):
        self.serial = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=8,
            parity='N',
            stopbits=1,
            timeout=1
        )
        self.slave_id = slave_id

    def _calculate_crc(self, data):
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc.to_bytes(2, byteorder='little')

    def read_holding_registers(self, address, count=1):
        # 构建Modbus RTU读取报文
        # Slave ID + Function Code (03) + Address (2 bytes) + Count (2 bytes)
        message = bytes([
            self.slave_id,  # Slave ID
            0x03,          # Function Code (Read Holding Registers)
            address >> 8,  # Address High Byte
            address & 0xFF,# Address Low Byte
            count >> 8,    # Count High Byte
            count & 0xFF   # Count Low Byte
        ])
        
        # 添加CRC校验
        message += self._calculate_crc(message)
        
        # 发送请求
        self.serial.write(message)
        
        # 读取响应
        response = self.serial.read(5 + count * 2)  # 1(ID) + 1(FC) + 1(Byte Count) + 2*count(Data) + 2(CRC)
        
        if len(response) < 5:
            raise Exception("No response from device")
            
        # 检查响应的CRC
        received_crc = response[-2:]
        calculated_crc = self._calculate_crc(response[:-2])
        if received_crc != calculated_crc:
            raise Exception("CRC check failed")
            
        # 解析数据
        data = response[3:-2]  # 跳过ID、功能码、字节数，以及CRC
        value = int.from_bytes(data, byteorder='big')
        return value

def main():
    # 创建Modbus RTU实例
    modbus = ModbusRTU(port="/dev/ttySC0", baudrate=9600, slave_id=1)
    
    try:
        while True:
            try:
                # 读取压力值 (地址40001，内部地址0)
                pressure = modbus.read_holding_registers(0)
                print(f"压力值: {pressure}")
            except Exception as e:
                print(f"读取错误: {e}")
            
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n程序已停止")
    finally:
        modbus.serial.close()

if __name__ == "__main__":
    main() 