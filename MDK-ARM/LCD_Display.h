#ifndef HEADERS
#define HEADERS
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"
#include <stdio.h>
#endif

static GPIO_TypeDef* PORT_RS_and_E;							  // RS and E PORT
static uint16_t PIN_RS, PIN_E;									  // RS and E pins
static GPIO_TypeDef* PORT_LSB;										// LSBs D0, D1, D2 and D3 PORT
static uint16_t D0_PIN, D1_PIN, D2_PIN, D3_PIN;	// LSBs D0, D1, D2 and D3 pins
static GPIO_TypeDef* PORT_MSB;										// MSBs D4, D5, D6 and D7 PORT
static uint16_t D4_PIN, D5_PIN, D6_PIN, D7_PIN;	// MSBs D4, D5, D6 and D7 pins

// Public functions
void LCD1602_setGPIOPorts(GPIO_TypeDef* PORT_RS_E, uint16_t RS, uint16_t E, GPIO_TypeDef* PORT_LSBs0to3, uint16_t D0, uint16_t D1, uint16_t D2, uint16_t D3, GPIO_TypeDef* PORT_MSBs4to7, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7);
void LCD1602_Begin8BIT(void);
void LCD1602_print(char string[]);
void LCD1602_1stLine(void);
void LCD1602_2ndLine(void);
void LCD1602_clear(void);
void LCD1602_writeData(uint8_t data);
void LCD1602_PrintInt(int num);
void LCD1602_PrintDecimal(float num);
void LCD1602_PrintLoadingPerc(int percentage);
