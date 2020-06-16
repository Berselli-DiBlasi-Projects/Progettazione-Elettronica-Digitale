#include "MQ5.h"
#include "LCD_Display.h"

void check_gas(ADC_HandleTypeDef hadc1, IWDG_HandleTypeDef hiwdg)
{
	int val;
	int cond;
	cond = HAL_GPIO_ReadPin(GPIOB,GAS_FLAG_Pin);
	while(cond)
	{	
		HAL_IWDG_Refresh(&hiwdg);
		hiwdg.Instance->PR = hiwdg.Init.Prescaler;
		hiwdg.Instance->RLR = hiwdg.Init.Reload;
		
		val = 0;
		HAL_ADC_Start(&hadc1);
		HAL_GPIO_WritePin(GPIOB,BUZZ_Pin,GPIO_PIN_SET);
		LCD1602_1stLine();
		LCD1602_print("Gas:            ");
		LCD1602_2ndLine();
		
		HAL_ADC_PollForConversion(&hadc1,1000);
		for(int i = 0; i < MSMQ5; i++)
		{
			val += HAL_ADC_GetValue(&hadc1);
		}
		val /= MSMQ5;
		LCD1602_PrintInt(val);
		LCD1602_print(" ppm     ");
		DWT_Delay_us (2000000);
		
		if(val < THRESHOLD)
		{
			cond=0;
			HAL_GPIO_WritePin(GPIOB,BUZZ_Pin,GPIO_PIN_RESET);
			
			HAL_IWDG_Refresh(&hiwdg);
			hiwdg.Instance->PR = hiwdg.Init.Prescaler;
			hiwdg.Instance->RLR = hiwdg.Init.Reload;
		}
	}
}
