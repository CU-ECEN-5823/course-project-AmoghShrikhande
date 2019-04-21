/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>


#define	LED0_port gpioPortF
#define LED0_pin	4
#define LED1_port gpioPortF
#define LED1_pin 5

void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	// for PB0
	GPIO_PinModeSet(gpioPortF, 6, gpioModeInputPull, 1);

	// for PB1
	GPIO_PinModeSet(gpioPortF, 7, gpioModeInputPull, 1);

	GPIO_PinModeSet(FLAME_SENSOR_PORT, FLAME_SENSOR_PIN, gpioModeInputPull, 1); // PIN 7

	GPIO_PinModeSet(ALARM_PORT, ALARM_PIN, gpioModePushPull, 0);
}

void gpio_interrupt_start()
{
	// PB0 interrupt setup
	GPIO_IntConfig(gpioPortF,6,0,1,1);

	// Flame sensor gpio interrupt configuration
	GPIO_IntConfig(FLAME_SENSOR_PORT,FLAME_SENSOR_PIN,0,1,1);

	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
//	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void GPIO_EVEN_IRQHandler(void)
{
	uint32_t reason = GPIO_IntGet();
	GPIO_IntClear(reason);
	if(reason == 0x40)
		gecko_external_signal(PUSHBUTTON_FLAG);

	if(reason == 0x400)
		gecko_external_signal(FLAME_SENSOR_FLAG);
}

//void GPIO_ODD_IRQHandler(void)
//{
//	uint32_t reason = GPIO_IntGet();
//	GPIO_IntClear(reason);
//}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

/* For display */
// toggling of EXTCOMIN pin
void gpioSetDisplayExtcomin(bool amogh ){
	if(amogh == false)
		GPIO_PinOutClear(gpioPortD, 13);

	else
		GPIO_PinOutSet(gpioPortD, 13);
}

// display pin EXTCOMIN enable
void gpioEnableDisplay(){
	GPIO_PinOutSet(gpioPortD, 15);
}
