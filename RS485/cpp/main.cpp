#include <iostream>
#include <signal.h>
#include <cstring>
#include "DEV_Config_wrapper.h"

void signalHandler(int signo) {
    std::cout << "\r\nHandler: Program stop\r\n";
    DEV_Config::exit();
    exit(0);
}

int main(int argc, char** argv) {
    // 注册信号处理
    signal(SIGINT, signalHandler);
    
    // 初始化模块
    if (DEV_Config::init()) {
        return 1;
    }
    
    // 初始化UART
    DEV_Config::uartInit("/dev/ttySC0");
    
    // 准备缓冲区
    uint8_t pData[100] = {0};
    int i = 0;
    
    // 发送欢迎消息
    DEV_Config::digitalWrite(TXDEN_1, 0);  // 设置为发送模式
    const char* welcome = "Waveshare RS485 CAN HAT B\r\n";
    DEV_Config::uartWriteBytes((uint8_t*)welcome, strlen(welcome));
    DEV_Config::delay(5);  // 等待发送完成
    DEV_Config::digitalWrite(TXDEN_1, 1);  // 设置为接收模式
    
    // 主循环
    while (true) {
        pData[i] = DEV_Config::uartReadByte();
        if (pData[i] == '\n') {
            // 收到换行符，回显数据
            DEV_Config::digitalWrite(TXDEN_1, 0);  // 设置为发送模式
            for (int j = 0; j < i + 1; j++) {
                std::cout << (char)pData[j];
            }
            DEV_Config::uartWriteBytes(pData, i + 1);
            DEV_Config::delay(5);  // 等待发送完成
            DEV_Config::digitalWrite(TXDEN_1, 1);  // 设置为接收模式
            i = 0;
        } else if (i >= 100) {
            i = 0;
        } else {
            i++;
        }
    }
    
    DEV_Config::exit();
    return 0;
} 