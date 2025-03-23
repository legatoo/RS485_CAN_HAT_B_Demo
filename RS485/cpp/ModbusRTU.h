#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <string>
#include <vector>
#include <stdexcept>
#include "DEV_Config_wrapper.h"

class ModbusRTU {
public:
    ModbusRTU(const std::string& port = "/dev/ttySC0", 
               int baudrate = 9600, 
               uint8_t slave_id = 1) 
        : slave_id(slave_id) {
        if (!DEV_Config::init()) {
            throw std::runtime_error("Failed to initialize device");
        }
        DEV_Config::uartInit(port.c_str());
        DEV_Config::uartSetBaudrate(baudrate);
    }

    ~ModbusRTU() {
        DEV_Config::exit();
    }

    // 读取保持寄存器
    uint16_t readHoldingRegister(uint16_t address, uint16_t count = 1) {
        std::vector<uint8_t> request = createReadRequest(address, count);
        std::vector<uint8_t> response = sendRequest(request);
        return parseReadResponse(response);
    }

private:
    uint8_t slave_id;

    // 创建读取请求
    std::vector<uint8_t> createReadRequest(uint16_t address, uint16_t count) {
        std::vector<uint8_t> request = {
            slave_id,           // 从站地址
            0x03,              // 功能码：读保持寄存器
            (uint8_t)(address >> 8),    // 起始地址高字节
            (uint8_t)(address & 0xFF),  // 起始地址低字节
            (uint8_t)(count >> 8),      // 寄存器数量高字节
            (uint8_t)(count & 0xFF)     // 寄存器数量低字节
        };
        
        // 添加CRC校验
        auto crc = calculateCRC(request);
        request.insert(request.end(), crc.begin(), crc.end());
        return request;
    }

    // 发送请求并接收响应
    std::vector<uint8_t> sendRequest(const std::vector<uint8_t>& request) {
        // 设置发送模式
        DEV_Config::digitalWrite(TXDEN_1, 0);
        
        // 发送数据
        DEV_Config::uartWriteBytes((uint8_t*)request.data(), request.size());
        DEV_Config::delay(5);  // 等待发送完成
        
        // 切换到接收模式
        DEV_Config::digitalWrite(TXDEN_1, 1);
        
        // 读取响应
        std::vector<uint8_t> response;
        int timeout = 1000;
        while (timeout-- > 0) {
            int byte = DEV_Config::uartReadByte();
            if (byte >= 0) {
                response.push_back(byte);
                if (response.size() >= 5) {  // 最小响应长度
                    break;
                }
            }
            DEV_Config::delay(1);
        }
        
        return response;
    }

    // 解析读取响应
    uint16_t parseReadResponse(const std::vector<uint8_t>& response) {
        if (response.size() < 5) {
            throw std::runtime_error("Invalid response length");
        }
        
        if (response[0] != slave_id || response[1] != 0x03) {
            throw std::runtime_error("Invalid response header");
        }
        
        uint16_t value = (response[3] << 8) | response[4];  // 高字节在前
        return value;
    }

    // 计算CRC校验
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
        return {(uint8_t)(crc & 0xFF), (uint8_t)(crc >> 8)};
    }
};

#endif // MODBUS_RTU_H 