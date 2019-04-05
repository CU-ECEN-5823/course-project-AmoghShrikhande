/*
 * scheduler.h
 *
 *  Created on: Apr 4, 2019
 *      Author: amosh
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "native_gecko.h"

// soft timer flags
#define DISPLAY_REFRESH 0x05
#define LOG_REFRESH 0x06
#define TIMER_ID_FACTORY_RESET 0x07
#define TIMER_ID_RESTART 0x08

// external event flag
#define PUSHBUTTON_FLAG 0x13

uint32_t tickCount;

#endif /* SRC_SCHEDULER_H_ */
