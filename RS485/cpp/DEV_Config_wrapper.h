#ifndef DEV_CONFIG_WRAPPER_H
#define DEV_CONFIG_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lib/Config/DEV_Config.h"

#ifdef __cplusplus
}
#endif

class DEV_Config {
public:
    static bool init() {
        return DEV_ModuleInit() == 1;
    }

    static void exit() {
        DEV_ModuleExit();
    }

    static void gpioMode(uint16_t pin, uint16_t mode) {
        DEV_GPIO_Mode(pin, mode);
    }

    static void digitalWrite(uint16_t pin, uint8_t value) {
        DEV_Digital_Write(pin, value);
    }

    static uint8_t digitalRead(uint16_t pin) {
        return DEV_Digital_Read(pin);
    }

    static void delay(uint32_t ms) {
        DEV_Delay_ms(ms);
    }

    static void uartInit(const char* device) {
        DEV_UART_Init((char*)device);
    }

    static void uartWriteByte(uint8_t data) {
        UART_Write_Byte(data);
    }

    static int uartReadByte() {
        return UART_Read_Byte();
    }

    static void uartSetBaudrate(uint32_t baudrate) {
        UART_Set_Baudrate(baudrate);
    }

    static int uartWriteBytes(uint8_t* data, uint32_t len) {
        return UART_Write_nByte(data, len);
    }
};

#endif // DEV_CONFIG_WRAPPER_H 