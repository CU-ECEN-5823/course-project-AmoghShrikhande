/*
 * Event_handler.h
 *
 *  Created on: 13-Feb-2019
 *      Author: AMOGH
 */

#ifndef SRC_EVENT_HANDLER_H_
#define SRC_EVENT_HANDLER_H_

#include <stdbool.h>
#include "gpio.h"
#include "I2C.h"


// Structure of possible events which can cause transition in states
struct Possible_events{
	bool timer_UF;                    // This event occurs when an underflow is reached
	bool sensor_status;
	bool application_upload;
	bool sensor_mode_set;
	bool value_calculated;
	bool meas_mode_data_read;
	bool hardware_id_pass;
	bool event_null;
};

struct Possible_events event_set;

void scheduler(void);

#endif /* SRC_EVENT_HANDLER_H_ */
