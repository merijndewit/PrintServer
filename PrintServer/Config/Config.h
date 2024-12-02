#pragma once 

//wifi
#define USER_WIFI_SSID "enter-your-wifi-ssid"
#define USER_WIFI_PASS "ender-your-wifi0password"

#define MAX_CONNECTION_RETRIES 6

#define DEBUG_NAME "PrintServer"

#define MAX_GCODE_LENGTH 256

//define the sd card interface

//#define SD_INTERFACE_SPI
#define SD_INTERFACE_4BIT

// sd card pinout
#ifdef SD_INTERFACE_SPI
    #define PIN_NUM_MISO 13
    #define PIN_NUM_CLK  12
    #define PIN_NUM_MOSI 11
    #define PIN_NUM_CS   10
#endif

#ifdef SD_INTERFACE_4BIT
    #define SD_CLOCK_PIN GPIO_NUM_6
    #define SD_CMD_PIN GPIO_NUM_11
    #define SD_D0 GPIO_NUM_7
    #define SD_D1 GPIO_NUM_8
    #define SD_D2 GPIO_NUM_9
    #define SD_D3 GPIO_NUM_10
#endif

//UART
#define TXD_PIN (GPIO_NUM_19)  // Define UART TX pin
#define RXD_PIN (GPIO_NUM_18)  // Define UART RX pin
#define UART_PORT_NUM UART_NUM_2  // Use UART1
#define UART_BAUD_RATE 9600  // Match printer's baud rate
#define UART_BUF_SIZE (1024)