/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include <native_gecko.h>
#include "scheduler.h"

//for LCD
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1

// flame sensor gpio
#define FLAME_SENSOR_PORT gpioPortD
#define FLAME_SENSOR_PIN 10

// alarm buzzer gpio
#define ALARM_PORT gpioPortA
#define ALARM_PIN 3

// functions for LCD
void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);

void GPIO_EVEN_IRQHandler(void);
void gpio_interrupt_start();

void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
#endif /* SRC_GPIO_H_ */
