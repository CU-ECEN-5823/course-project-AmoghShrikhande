/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

/***************************************************************************************
 *                          HEADERS                                                     *
 ***************************************************************************************/
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>

/***************************************************************************************
 *                          MACROS                                                     *
 ***************************************************************************************/
// On board LED0 is on GPIO Port F, PIN 4
#define	LED0_port gpioPortF
#define LED0_pin	4
// On board LED0 is on GPIO Port F, PIN 5
#define LED1_port gpioPortF
#define LED1_pin 5

#define SENSOR_PORT gpioPortA
#define SENSOR_PIN 3

/* Initialize gpio pins */
void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	// for PB0
	GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInputPull, 1);

	// for Sensor enable
	GPIO_PinModeSet(SENSOR_PORT, SENSOR_PIN, gpioModePushPull, 0);

	// for PB1
	GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInputPull, 1);

	// for flame sensor
	GPIO_PinModeSet(FLAME_SENSOR_PORT, FLAME_SENSOR_PIN, gpioModeInputPull, 1); // PIN 7

	// for buzzer
	GPIO_PinModeSet(ALARM_PORT, ALARM_PIN, gpioModePushPull, 0);
}

/* Start gpio interrupts */
void gpio_interrupt_start()
{
	// PB0 interrupt setup
	GPIO_IntConfig(PB0_PORT,PB0_PIN,0,1,1);

	// Flame sensor gpio interrupt configuration
	GPIO_IntConfig(FLAME_SENSOR_PORT,FLAME_SENSOR_PIN,0,1,1);

	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	//	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/* GPIO EVEN interrupt handler
 * - handles PB0 interrupt
 * - handles Flame sensor interrupt
 *  */
void GPIO_EVEN_IRQHandler(void)
{
	uint32_t reason = GPIO_IntGet();
	GPIO_IntClear(reason);

	// push button interrupt
	if(reason == 0x40)
		gecko_external_signal(PUSHBUTTON_FLAG);

	// flame sensor interrupt
	if(reason == 0x400)
		gecko_external_signal(FLAME_SENSOR_FLAG);
}

//void GPIO_ODD_IRQHandler(void)
//{
//	uint32_t reason = GPIO_IntGet();
//	GPIO_IntClear(reason);
//}

// lights ON
void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}

// lights OFF
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}

// alert LED ON
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}

// alert LED OFF
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
