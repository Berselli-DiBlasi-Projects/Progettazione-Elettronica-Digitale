#ifndef HEADERS
#define HEADERS
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"
#include <stdio.h>
#endif

// Define number of measurements combined in dht11 (Min. 5, Max. 20)
#define MSDHT11 10

struct dht11_measurement {
	uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2; // Measurement values
	uint16_t sum; // Variable to check if the measurement is correct
};

struct dht11_measurements {
	//Mean of values, next_index to use, size of arrays measurements
	float Rh[MSDHT11], Temp[MSDHT11];
	float Rh_mean, Temp_mean;
	int next_index, size;
};

//static uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
//static uint16_t sum, RH, TEMP;
static int temp_low, temp_high, rh_low, rh_high;
static char temp_char1[2], temp_char2, rh_char1[2], rh_char2;
static uint8_t check = 0;
static GPIO_InitTypeDef GPIO_InitStruct;

void dht11_measurements_init(struct dht11_measurements* u);
int dht11_measurements_addvalue(struct dht11_measurements* u);
int dht11_measurements_mean(struct dht11_measurements* u);
void print_dht11_measurements(struct dht11_measurements* u);
