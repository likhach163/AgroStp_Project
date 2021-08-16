/*
 * Sensor.h
 *
 *  Created on: May 5, 2021
 *      Author: Лихачев Иван
 */

#ifndef SENSOR_DRIVER_SENSOR_H_
#define SENSOR_DRIVER_SENSOR_H_

#define I2C_ADDRESS_UPPER               0x64 //Default Address
#define I2C_ADDRESS_MIDDLE              0x65
#define I2C_ADDRESS_LOWER               0x66

#define START_TRANSMISSION              0x10
#define STOP_TRANSMISSION               0x30

#define TEMPERATURE_MEASURE             0x22
#define MOISTURE_MEASURE                0x24

#define ADDRESS_ERROR                   0x40
#define TEMPERATURE_MEASURE_ERROR       0x42
#define MOISTURE_MEASURE_ERROR          0x44

void initI2CAddress(void);
void initSTOPMode(void);
void goToSTOPMode(void);
void waitForClickButton(void);

#endif /* SENSOR_DRIVER_SENSOR_H_ */
