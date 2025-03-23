#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <vector>
#include <stdexcept>
#include "DEV_Config_wrapper.h"

class ModbusRTU {
public:
    ModbusRTU(const char* port = "/dev/ttySC0", int baudrate = 9600, uint8_t slave_id = 1)
        : slave_id(slave_id) {
        if (DEV_Config::init()) {
            throw std::runtime_error("Failed to initialize device");
        }
        DEV_Config::uartInit(port);
        DEV_Config::uartSetBaudrate(baudrate);
    }

    ~ModbusRTU() {
        DEV_Config::exit();
    }

    uint16_t readHoldingRegisters(uint16_t address, uint16_t count = 1) {
        // 构建Modbus RTU读取报文
        std::vector<uint8_t> message = {
            slave_id,           // Slave ID
            0x03,              // Function Code (Read Holding Registers)
            static_cast<uint8_t>(address >> 8),    // Address High Byte
            static_cast<uint8_t>(address & 0xFF),  // Address Low Byte
            static_cast<uint8_t>(count >> 8),      // Count High Byte
            static_cast<uint8_t>(count & 0xFF)     // Count Low Byte
        };

        // 计算并添加CRC
        auto crc = calculateCRC(message);
        message.insert(message.end(), crc.begin(), crc.end());

        // 发送请求
        DEV_Config::digitalWrite(TXDEN_1, 0);  // 设置为发送模式
        DEV_Config::uartWriteBytes(message.data(), message.size());
        DEV_Config::delay(5);  // 等待发送完成
        DEV_Config::digitalWrite(TXDEN_1, 1);  // 设置为接收模式

        // 读取响应
        std::vector<uint8_t> response;
        int expected_length = 5 + count * 2;  // 1(ID) + 1(FC) + 1(Byte Count) + 2*count(Data) + 2(CRC)
        int timeout = 1000;  // 1秒超时

        while (timeout-- > 0 && response.size() < expected_length) {
            int byte = DEV_Config::uartReadByte();
            if (byte >= 0) {
                response.push_back(static_cast<uint8_t>(byte));
            }
            DEV_Config::delay(1);
        }

        if (response.size() < 5) {
            throw std::runtime_error("No response from device");
        }

        // 检查CRC
        std::vector<uint8_t> received_data(response.begin(), response.end() - 2);
        auto received_crc = std::vector<uint8_t>(response.end() - 2, response.end());
        auto calculated_crc = calculateCRC(received_data);
        
        if (received_crc != calculated_crc) {
            throw std::runtime_error("CRC check failed");
        }

        // 解析数据
        uint16_t value = (response[3] << 8) | response[4];
        return value;
    }

private:
    uint8_t slave_id;

    std::vector<uint8_t> calculateCRC(const std::vector<uint8_t>& data) {
        uint16_t crc = 0xFFFF;
        for (uint8_t byte : data) {
            crc ^= byte;
            for (int i = 0; i < 8; i++) {
                if (crc & 0x0001) {
                    crc = (crc >> 1) ^ 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        return {static_cast<uint8_t>(crc & 0xFF), 
                static_cast<uint8_t>(crc >> 8)};
    }
};

#endif // MODBUS_RTU_H 