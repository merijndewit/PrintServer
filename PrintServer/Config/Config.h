#pragma once 

//wifi
#define USER_WIFI_SSID "enter-your-wifi-ssid"
#define USER_WIFI_PASS "ender-your-wifi0password"

#define MAX_CONNECTION_RETRIES 6

#define DEBUG_NAME "PrintServer"

//sd card


//UART
#define TXD_PIN (GPIO_NUM_19)  // Define UART TX pin
#define RXD_PIN (GPIO_NUM_18)  // Define UART RX pin
#define UART_PORT_NUM UART_NUM_2  // Use UART1
#define UART_BAUD_RATE 9600  // Match printer's baud rate
#define UART_BUF_SIZE (1024)