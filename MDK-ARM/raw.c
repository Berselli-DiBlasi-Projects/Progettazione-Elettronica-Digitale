#include <stdlib.h>
#include <stdbool.h>

static GPIO_TypeDef* PORT_RS_and_E;							  // RS and E PORT
static uint16_t PIN_RS, PIN_E;									  // RS and E pins
static GPIO_TypeDef* PORT_LSB;										// LSBs D0, D1, D2 and D3 PORT
static uint16_t D0_PIN, D1_PIN, D2_PIN, D3_PIN;	// LSBs D0, D1, D2 and D3 pins
static GPIO_TypeDef* PORT_MSB;										// MSBs D5, D6, D7 and D8 PORT
static uint16_t D4_PIN, D5_PIN, D6_PIN, D7_PIN;	// MSBs D5, D6, D7 and D8 pins

void setDisplayGPIOPorts(GPIO_TypeDef* PORT_RS_E, uint16_t RS, uint16_t E, GPIO_TypeDef* PORT_LSBs0to3, uint16_t D0, uint16_t D1, uint16_t D2, uint16_t D3, GPIO_TypeDef* PORT_MSBs4to7, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7)
{
	//Set GPIO Ports and Pins data
	PORT_RS_and_E = PORT_RS_E;
	PIN_RS = RS;
	PIN_E = E;
	PORT_LSB = PORT_LSBs0to3;
	D0_PIN = D0;
	D1_PIN = D1;
	D2_PIN = D2;
	D3_PIN = D3;
	PORT_MSB = PORT_MSBs4to7;
	D4_PIN = D4;
	D5_PIN = D5;
	D6_PIN = D6;
	D7_PIN = D7;
}

int main_func()
{
	// START INITIALIZE DISPLAY
	
	//Initialise microsecond timer
	LCD1602_TIM_Config();
	//Set the mode to 8 bits
	mode_8_4_I2C = 1;
	//Function set variable to 8 bits mode
	FunctionSet = 0x38;
	
	//Initialise LCD
	//1. Wait at least 15ms
	HAL_Delay(20);
	//2. Attentions sequence
	LCD1602_writeCommand(0x30);
	HAL_Delay(5);
	LCD1602_writeCommand(0x30);
	HAL_Delay(1);
	LCD1602_writeCommand(0x30);
	HAL_Delay(1);
	//3. Function set; Enable 2 lines, Data length to 8 bits
	LCD1602_writeCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N | LCD_FUNCTION_DL);
	//4. Display control (Display ON, Cursor ON, blink cursor)
	LCD1602_writeCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
	//5. Clear LCD and return home
	LCD1602_writeCommand(LCD_CLEARDISPLAY);
	HAL_Delay(2);
	
	// END INITIALIZE DISPLAY
}