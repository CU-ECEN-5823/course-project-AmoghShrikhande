/*
 * Low_energy_timer.h
 *
 *  Created on: 30-Jan-2019
 *      Author: AMOGH
 */

#ifndef SRC_LOW_ENERGY_TIMER_H_
#define SRC_LOW_ENERGY_TIMER_H_
#include "log.h"


uint8_t Roll_off;          // Variable for time stamp

void leTimer_config(void);
void LETIMER0_IRQHandler(void);

//#define MODE_EM1

#define SLEEP_MODE 3    //    Define the desired energy mode here



#endif /* SRC_LOW_ENERGY_TIMER_H_ */
