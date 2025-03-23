#include <iostream>
#include <signal.h>
#include "ModbusRTU.h"

ModbusRTU* g_modbus = nullptr;

void signalHandler(int signo) {
    std::cout << "\r\n程序已停止\r\n";
    if (g_modbus) {
        delete g_modbus;
    }
    exit(0);
}

int main(int argc, char** argv) {
    // 注册信号处理
    signal(SIGINT, signalHandler);
    
    try {
        // 创建ModbusRTU实例
        g_modbus = new ModbusRTU("/dev/ttySC0", 9600, 1);
        
        while (true) {
            try {
                // 读取压力值 (地址40001，内部地址0)
                uint16_t pressure = g_modbus->readHoldingRegisters(0);
                std::cout << "压力值: " << pressure << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "读取错误: " << e.what() << std::endl;
            }
            
            DEV_Config::delay(1000);  // 延时1秒
        }
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 