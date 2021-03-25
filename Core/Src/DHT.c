#include "DHT.h"

#define lineDown() 	HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_RESET)
#define lineUp()		HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET)
#define getLine()		(HAL_GPIO_ReadPin(DHT_Port, DHT_Pin) == GPIO_PIN_SET)
#define Delay(d)		HAL_Delay(d)

static void goToOutput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = DHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	#if DHT_PullUp == 1
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	#else
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	#endif
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct);
}

static void goToInput(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = DHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	#if DHT_PullUp == 1
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	#else
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	#endif
  HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct);
}

DHT_data DHT_getData(DHT_type t) {
	DHT_data data = {0.0f, 0.0f};

	goToOutput();

	lineDown();
	Delay(15);

	lineUp();
	goToInput();


	uint16_t timeout = 0;

	while(getLine()) {
		timeout++;
		if (timeout > DHT_timeout) return data;
	}
	timeout = 0;

	while(!getLine()) {
		timeout++;
		if (timeout > DHT_timeout) return data;
	}
	timeout = 0;

	while(getLine()) {
		timeout++;
		if (timeout > DHT_timeout) return data;
	}


	uint8_t rawData[5] = {0,0,0,0,0};
	for(uint8_t a = 0; a < 5; a++) {
		for(uint8_t b = 7; b != 255; b--) {
			uint32_t hT = 0, lT = 0;

			while(!getLine()) lT++;

			while(getLine()) hT++;

			if(hT > lT) rawData[a] |= (1<<b);
		}
	}

	if((uint8_t)(rawData[0] + rawData[1] + rawData[2] + rawData[3]) == rawData[4]) {

		if (t == DHT11) {
			data.hum = (float)rawData[0];
			data.temp = (float)rawData[2];;
		}
	}

	return data;
}
