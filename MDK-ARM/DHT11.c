#include "DHT11.h"
#include "LCD_Display.h"

//Private functions
void set_gpio_output(void)
{
	/*Configure GPIO pin output: PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void set_gpio_input(void)
{
	/*Configure GPIO pin input: PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//Public functions
void DHT11_start(void)
{
	set_gpio_output ();  // set the pin as output
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, (GPIO_PinState)0);   // pull the pin low
	DWT_Delay_us (18000);   // wait for 18ms
	//HAL_Delay(18);
	set_gpio_input ();   // set as input
}

void check_response(void)
{
	DWT_Delay_us (40);
	//HAL_Delay(4);
	if (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)))
	{
		DWT_Delay_us (80);
		//HAL_Delay(8);
		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1))) check = 1;
	}
	while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go low
}

uint8_t dht11_read(void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go high
		DWT_Delay_us (40);   // wait for 40 us
		//HAL_Delay(40);
		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)) == 0)   // if the pin is low 
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));  // wait for the pin to go low
	}
	return i;
}

/*
  Init an empty struct dht11_measurements.
*/
void dht11_measurements_init(struct dht11_measurements* u)
{
	u->Rh_mean = 0;
	u->Temp_mean = 0;
	u->next_index = 0;
	u->size = 0;
	
	DWT_Delay_us (500000);
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("Inizializzazione");
	DWT_Delay_us (500000);
	
	for(int i = 0; i < MSDHT11; i++)
	{
		LCD1602_clear();
		LCD1602_1stLine();
		LCD1602_print("Inizializzazione");
		LCD1602_2ndLine();
		LCD1602_PrintLoadingPerc(i * 100 / MSDHT11);
		DWT_Delay_us (1000000);
		dht11_measurements_addvalue(u);
	}
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("Inizializzazione");
	LCD1602_2ndLine();
	LCD1602_PrintLoadingPerc(100);
	DWT_Delay_us (1000000);
	dht11_measurements_mean(u);
}

/*
  Add a single measure to measurements and update the mean values.
*/
int dht11_measurements_addvalue(struct dht11_measurements* u)
{
	struct dht11_measurement m;
	uint16_t sum = 0;
	float tmp, Rh_old, Temp_old;
	
	DHT11_start();
	check_response();
	m.Rh_byte1 = dht11_read();
	m.Rh_byte2 = dht11_read();
	m.Temp_byte1 = dht11_read();
	m.Temp_byte2 = dht11_read();
	m.sum = dht11_read();
	
	// Calculate total sum in a separate way.
	sum += m.Rh_byte1;
	sum += m.Rh_byte2;
	sum += m.Temp_byte1;
	sum += m.Temp_byte2;
	// if the data is correct, add measure to measurements
	if (m.sum == sum)
	{
		DWT_Delay_us (500000);
		
		if(u->size == MSDHT11)
		{
			Rh_old = u->Rh[u->next_index];
			Temp_old = u->Temp[u->next_index];
		}
		
		// Update arrays measurements values
		u->Rh[u->next_index] = m.Rh_byte1;
		tmp = m.Rh_byte2;
		if(tmp < 10)
		{
			tmp *= 0.1f;
		}
		else if(tmp < 100)
		{
			tmp *= 0.01f;
		}
		else
		{
			tmp *= 0.001f;
		}
		u->Rh[u->next_index] += tmp;
		u->Temp[u->next_index] = m.Temp_byte1;
		tmp = m.Temp_byte2;
		if(tmp < 10)
		{
			tmp *= 0.1f;
		}
		else if(tmp < 100)
		{
			tmp *= 0.01f;
		}
		else
		{
			tmp *= 0.001f;
		}
		u->Temp[u->next_index] += tmp;
		
		// Update mean
		if(u->size == MSDHT11)
		{
			u->Rh_mean *= MSDHT11;
			u->Temp_mean *= MSDHT11;
			
			u->Rh_mean -= Rh_old;
			u->Temp_mean -= Temp_old;
			
			u->Rh_mean += u->Rh[u->next_index];
			u->Temp_mean += u->Temp[u->next_index];
			
			u->Rh_mean /= MSDHT11;
			u->Temp_mean /= MSDHT11;
		}
		
		if(u->size < MSDHT11)
			u->size++;
		
		u->next_index++;
		if(u->next_index == MSDHT11)
			u->next_index = 0;
		
		return 0;
	}
	else
	{
		LCD1602_clear();
		LCD1602_1stLine();
		LCD1602_print("ERRORE");
		LCD1602_2ndLine();
		LCD1602_print("Sensore DHT11");
		DWT_Delay_us (1000000);
		LCD1602_clear();
		return -1;
	}
}

/*
  Mean of all values stored in struct dht11 measurements.
*/
int dht11_measurements_mean(struct dht11_measurements* u)
{
	if(u->size == MSDHT11)
	{
		for(int i = 0; i < MSDHT11; i++)
		{
			u->Rh_mean += u->Rh[i];
			u->Temp_mean += u->Temp[i];
		}
		
		u->Rh_mean /= MSDHT11;
		u->Temp_mean /= MSDHT11;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

// Print on display dht11 measurements
void print_dht11_measurements(struct dht11_measurements* u)
{
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("Umidita':");
	LCD1602_2ndLine();
	LCD1602_PrintDecimal(u->Rh_mean);
	LCD1602_print(" %");
	DWT_Delay_us (2000000);
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("Temperatura:");
	LCD1602_2ndLine();
	LCD1602_PrintDecimal(u->Temp_mean);
	LCD1602_print(" ");
	LCD1602_writeData(0xDF);
	LCD1602_print("C");
	DWT_Delay_us (2000000);
}
