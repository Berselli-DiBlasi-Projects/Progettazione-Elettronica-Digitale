#ifndef HEADERS
#define HEADERS
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"
#include <stdio.h>
#include "main.h"
#endif

// Define number of measurements combined in mq5 (Min. 1, Max. 10)
#define MSMQ5 5
// Define the gas alarm threshold
#define THRESHOLD 500

void check_gas(ADC_HandleTypeDef hadc1,IWDG_HandleTypeDef hiwdg);
