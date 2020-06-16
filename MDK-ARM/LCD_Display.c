#include "LCD_Display.h"

//Private functions
// Low level function to write on display.
static void LCD1602_write(uint8_t byte)
{
	HAL_GPIO_WritePin(PORT_LSB, D0_PIN, (GPIO_PinState)(byte&0x1));
	HAL_GPIO_WritePin(PORT_LSB, D1_PIN, (GPIO_PinState)(byte&0x2));
	HAL_GPIO_WritePin(PORT_LSB, D2_PIN, (GPIO_PinState)(byte&0x4));
	HAL_GPIO_WritePin(PORT_LSB, D3_PIN, (GPIO_PinState)(byte&0x8));
	
	HAL_GPIO_WritePin(PORT_MSB, D4_PIN, (GPIO_PinState)((byte>>4)&0x1));
	HAL_GPIO_WritePin(PORT_MSB, D5_PIN, (GPIO_PinState)((byte>>4)&0x2));
	HAL_GPIO_WritePin(PORT_MSB, D6_PIN, (GPIO_PinState)((byte>>4)&0x4));
	HAL_GPIO_WritePin(PORT_MSB, D7_PIN, (GPIO_PinState)((byte>>4)&0x8));
	
	//Write the Enable pulse
	HAL_GPIO_WritePin(PORT_RS_and_E, PIN_E, GPIO_PIN_SET);
	//set auto reload value for 1 sec
	TIM3->ARR = 10-1;

	TIM3->SR &= ~(0x0001);  // Clear UEV flag
	TIM3->CR1 |= 1UL;
	while((TIM3->SR&0x0001) != 1);
	HAL_GPIO_WritePin(PORT_RS_and_E, PIN_E, GPIO_PIN_RESET);
	TIM3->ARR = 60-1;
	TIM3->SR &= ~(0x0001);  // Clear UEV flag
	TIM3->CR1 |= 1UL;
	while((TIM3->SR&0x0001) != 1);
}

// Set the display to receive a command and then write it.
static void LCD1602_writeCommand(uint8_t command)
{
	//Set RS to 0
	HAL_GPIO_WritePin(PORT_RS_and_E, PIN_RS, GPIO_PIN_RESET);
	//write data to output pins
	LCD1602_write(command);
}

// Print n blank blocks on display.
void LCD1602_PrintBlanks(int n)
{
	for(int i = 0; i < n; i++)
	{
		LCD1602_writeData(0xFF);
	}
}

// Public functions

// Set the display to receive data and then write it.
void LCD1602_writeData(uint8_t data)
{
	//Set RS to 1
	HAL_GPIO_WritePin(PORT_RS_and_E, PIN_RS, GPIO_PIN_SET);
	//write data to output pins
	LCD1602_write(data);
}

// Set display GPIO ports on static variables.
// This function needs (in order):
// RS_GPIO_Port, RS_Pin, E_Pin, D0_GPIO_Port, D0_Pin, D1_Pin, D2_Pin, D3_Pin, D4_GPIO_Port, D4_Pin, D5_Pin, D6_Pin, D7_Pin
void LCD1602_setGPIOPorts(GPIO_TypeDef* PORT_RS_E, uint16_t RS, uint16_t E, GPIO_TypeDef* PORT_LSBs0to3, uint16_t D0, uint16_t D1, uint16_t D2, uint16_t D3, GPIO_TypeDef* PORT_MSBs4to7, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7)
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

// Initialize display to communicate in 8 bit parallel mode.
void LCD1602_Begin8BIT()
{
	//Initialise microsecond timer ()
	RCC_ClkInitTypeDef myCLKtypeDef;
	uint32_t clockSpeed;
	uint32_t flashLatencyVar;
	HAL_RCC_GetClockConfig(&myCLKtypeDef, &flashLatencyVar);
	if(myCLKtypeDef.APB1CLKDivider == RCC_HCLK_DIV1)
	{
		clockSpeed = HAL_RCC_GetPCLK1Freq();
	}
	else
	{
		clockSpeed = HAL_RCC_GetPCLK1Freq()*2;
	}
	clockSpeed *= 0.000001;
	
	//Enable clock for TIM2 timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // 0x1
	/*
	peripheral clock enable register
	Bit 1 TIM3EN: TIM3 clock enable
	This bit is set and cleared by software.
		0: TIM3 clock disabled
		1: TIM3 clock enabled
	*/

	//Set the mode to Count up
	TIM3->CR1 &= ~(0x0010);
	//Enable Update Event
	TIM3->CR1 &= ~(0x0001);
	
	/*
	UL : unsigned long int 
	1UL = 		0000 0000 0000 0000 0000 0000 0000 0001 
	1UL<<2 :4	0000 0000 0000 0000 0000 0000 0000 0100
	1UL<<3 :8	0000 0000 0000 0000 0000 0000 0000 1000
	*/

	//Update request source 
	TIM3->CR1 &= ~(1UL << 2);
	// Set bit 3 High to enable One-Pulse mode
	TIM3->CR1 |= (1UL << 3);				  
	//Set the Prescalar
	TIM3->PSC = clockSpeed-1;
	//Set and Auto-Reload Value to delay the timer 1 sec
	TIM3->ARR = 10-1;// The Flag sets when overflows
	//event generation register handling to reset the counter
	TIM3->EGR = 1; //Update generate auto
	/* 
	with this instroction we set the UG bit, Update generation
	Reinitialize the counter and generates an update of the registers. Note that the prescaler
	counter is cleared too (anyway the prescaler ratio is not affected). The counter is cleared if
	the center-aligned mode is selected or if DIR=0 (upcounting), else it takes the auto-reload
	value (TIMx_ARR) if DIR=1 (downcounting).
	*/
	TIM3->SR &= ~(0x0001);	//Clear Update interrupt flag
	
	//Initialise LCD
	//1. Wait at least 15ms
	HAL_Delay(20);
	//2. Function set; Enable 2 lines, Data length to 8 bits
	LCD1602_writeCommand(0x38);
	//3. Display control (Display ON, Cursor OFF, blink cursor OFF)
	LCD1602_writeCommand(0x0C);
	//4. Clear Display
	LCD1602_clear();
}

// Print a string on display.
void LCD1602_print(char string[])
{
	for(uint8_t i=0;  i< 16 && string[i]!=NULL; i++)
	{
		LCD1602_writeData((uint8_t)string[i]);
	}
}

// Set cursor to 1st line.
void LCD1602_1stLine(void)
{
	LCD1602_writeCommand(0x80);
}

// Set cursor to 2nd line.
void LCD1602_2ndLine(void)
{
	LCD1602_writeCommand(0xc0);
}

// Clear display.
void LCD1602_clear(void)
{
	LCD1602_writeCommand(0x01);
	HAL_Delay(3);
}

// Print an integer
void LCD1602_PrintInt(int num)
{
	char numStr[16];
	sprintf(numStr, "%d", num);
	LCD1602_print(numStr);
}

// Print a decimal number
void LCD1602_PrintDecimal(float num)
{
	char numStr[16];
	sprintf(numStr, "%.1f", num);
	LCD1602_print(numStr);
}

// Print loading row percentage
void LCD1602_PrintLoadingPerc(int percentage)
{
	int rest, value;
	rest = percentage % 10;
	value = percentage / 10;
	if(rest >= 5)
		value++;
	
	switch(value) {
		case 0:
			LCD1602_print("       ");
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 1:
			LCD1602_PrintBlanks(1);
			LCD1602_print("      ");
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 2:
			LCD1602_PrintBlanks(3);
			LCD1602_print("    ");
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 3:
			LCD1602_PrintBlanks(5);
			LCD1602_print("  ");
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 4:
			LCD1602_PrintBlanks(6);
			LCD1602_print(" ");
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 5:
			LCD1602_PrintBlanks(7); // 8
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 6:
			LCD1602_PrintBlanks(7);
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			break;
		case 7:
			LCD1602_PrintBlanks(7);
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			LCD1602_PrintBlanks(1);
			break;
		case 8:
			LCD1602_PrintBlanks(7);
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			LCD1602_PrintBlanks(3);
			break;
		case 9:
			LCD1602_PrintBlanks(7);
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			LCD1602_PrintBlanks(4);
			break;
		case 10:
			if(percentage == 100)
				LCD1602_PrintBlanks(6);
			else
				LCD1602_PrintBlanks(7);
			
			LCD1602_PrintInt(percentage);
			LCD1602_print("%");
			LCD1602_PrintBlanks(6);
			break;
		default:
			break;
	}
}
