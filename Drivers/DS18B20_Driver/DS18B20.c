/*
 * Ds18b20.c
 *
 *  Created on: May 5, 2021
 *      Author: Лихачев Иван
 */

#include "DS18B20.h"

#include "main.h"

GPIO_TypeDef *Port = {0};
uint16_t Pin = 0;

uint32_t DELAY_WAIT_CONVERT = 0;
uint32_t start_us = 0;

void ds18b20_init(DS18B20_RESOLUTION_t resolution, GPIO_TypeDef * port, uint16_t pin){
	__disable_irq();
	Port = port;
	Pin = pin;
	ds18b20_set_resolution(resolution);
	__enable_irq();
}

void ds18b20_convert_t(void) {
	reset();
	write_byte(SKIP_ROM);
	write_byte(CONVERT_T);
	start_us = HAL_GetTick();
}


uint32_t ds18b20_is_busy(void) {
	return (HAL_GetTick() - start_us) > DELAY_WAIT_CONVERT;
}

float ds18b20_read_temperature(void) {
	reset();
	write_byte(SKIP_ROM);
	write_byte(READ_SCRATCHPAD);
	return read_temperature();
}

void ds18b20_set_resolution(DS18B20_RESOLUTION_t resolution) {
	reset();
	write_byte(SKIP_ROM);
	write_byte(WRITE_SCRATCHPAD);
	write_byte(TH_REGISTER);
	write_byte(TL_REGISTER);
	write_byte(resolution);
	DELAY_WAIT_CONVERT = DELAY_T_CONVERT / get_divider(resolution);
}

void delay_us(uint32_t n) {
	TIM1->ARR = n - 1;
	TIM1->CNT = TIM1->ARR;
	TIM1->CR1 |= TIM_CR1_CEN; // start the timer
	while (TIM1->CNT != 0);
	TIM1->CR1 &= ~TIM_CR1_CEN; // stop the timer
}

void reset(void) {
	DQ_LOW(Port,Pin);
	delay_us(DELAY_RESET);
	DQ_HIGH(Port,Pin);
	delay_us(DELAY_RESET);
}

void write_bit(uint8_t bit) {
	DQ_LOW(Port,Pin);
	delay_us(bit ? DELAY_WRITE_1 : DELAY_WRITE_0);
	DQ_HIGH(Port,Pin);
	delay_us(bit ? DELAY_WRITE_1_PAUSE : DELAY_WRITE_0_PAUSE);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void write_byte(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		write_bit(data >> i & 1);
		delay_us(DELAY_T_PROTECTION);
	}
}

uint8_t get_divider(DS18B20_RESOLUTION_t resolution) {
	if (resolution == DS18B20_RESOLUTION_9_BIT) {
		return 8;
	} else if (resolution == DS18B20_RESOLUTION_10_BIT) {
		return 4;
	} else if (resolution == DS18B20_RESOLUTION_11_BIT) {
		return 2;
	} else {               //DS18B20_RESOLUTION_12_BIT
		return 1;
	}
}

float read_temperature(void) {
	int temperature=0;
	for (uint8_t i = 0; i < 16; i++) {
		DQ_LOW(Port,Pin);
		delay_us(DELAY_READ_SLOT);

		DQ_HIGH(Port,Pin);
		delay_us(DELAY_BUS_RELAX);

		temperature += READ_1(Port,Pin,i);
		delay_us(DELAY_READ_PAUSE);
	}
	return ((float)temperature / 16.0f);
}


float receiveDataDs18b20(){
	float temperature = 0;
	__disable_irq();

	ds18b20_convert_t();
	while(ds18b20_is_busy());
	temperature = ds18b20_read_temperature();

	__enable_irq();

	return temperature;
}
