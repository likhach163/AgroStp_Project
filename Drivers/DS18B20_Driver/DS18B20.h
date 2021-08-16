/*
 * Ds18b20.h
 *
 *  Created on: May 5, 2021
 *      Author: Лихачев Иван
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "main.h"

#define DELAY_RESET           500
#define DELAY_WRITE_0         60
#define DELAY_WRITE_0_PAUSE   10
#define DELAY_WRITE_1         10
#define DELAY_WRITE_1_PAUSE   60
#define DELAY_READ_SLOT       10
#define DELAY_BUS_RELAX       10
#define DELAY_READ_PAUSE      50
#define DELAY_T_CONVERT       760000
#define DELAY_T_PROTECTION    5

#define SKIP_ROM              0xCC
#define CONVERT_T             0x44
#define READ_SCRATCHPAD       0xBE
#define WRITE_SCRATCHPAD      0x4E
#define TH_REGISTER           0x4B
#define TL_REGISTER           0x46
#define DQ_LOW(Port,Pin) CLEAR_BIT(Port->ODR, Pin);
#define DQ_HIGH(Port,Pin) SET_BIT(Port->ODR, Pin);
#define READ_1(Port,Pin,x) ((uint16_t)(READ_BIT(Port->IDR, Pin) ? 1 : 0) << x)

typedef enum {
	DS18B20_RESOLUTION_9_BIT  = 0x1F,
	DS18B20_RESOLUTION_10_BIT = 0x3F,
	DS18B20_RESOLUTION_11_BIT = 0x5F,
	DS18B20_RESOLUTION_12_BIT = 0x7F
} DS18B20_RESOLUTION_t;

extern void ds18b20_init(DS18B20_RESOLUTION_t resolution, GPIO_TypeDef * port, uint16_t pin);
extern float receiveDataDs18b20();

extern void ds18b20_convert_t(void);
extern uint32_t ds18b20_is_busy(void);
extern float ds18b20_read_temperature(void);
extern void ds18b20_set_resolution(DS18B20_RESOLUTION_t resolution);
extern void delay_us(uint32_t n);
extern void reset(void);
extern void write_bit(uint8_t bit);
extern void write_byte(uint8_t data);
extern uint8_t get_divider(DS18B20_RESOLUTION_t resolution);
extern float read_temperature(void);
extern int initSensor();


#endif /* DS18B20TCLASS_H_ */
