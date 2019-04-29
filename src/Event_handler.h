/*
 * Event_handler.h
 *
 *  Created on: 26-April-2019
 *      Author: AMOGH
 */

#ifndef SRC_EVENT_HANDLER_H_
#define SRC_EVENT_HANDLER_H_



/***************************************************************************************
 *                          HEADERS & MACROS                                           *
 ***************************************************************************************/
#include <stdbool.h>
#include "gpio.h"
#include "I2C.h"

/***************************************************************************************
*                          GLOBAL STRUCTURES                                           *
***************************************************************************************/

// Structure of possible events which can cause transition in states
struct Possible_events{
	// Clock expires after 3 seconds
	bool timer_UF;
	// Check the status register of the sensors
	bool sensor_status;
	// To check if the new application is loaded in the sensor
	bool application_upload;
	// Set the sensor mode to mode 1 so that the sensor returns the raw value after every second
	bool sensor_mode_set;
	// The value of CO2 is received in ppm
	bool value_calculated;
	// Read the data from the MEAS_MODE register of the sensor
	bool meas_mode_data_read;
	// Check the Who Am I? register of the slave, it should return the value 0x81
	bool hardware_id_pass;
	// No event has occured
	bool event_null;
};

/***************************************************************************************
*                          GLOBAL INSTANCES                                           *
***************************************************************************************/
struct Possible_events event_set;

/***************************************************************************************
*                          FUNCTION PROTOTYPES                                         *
***************************************************************************************/
void scheduler(void);

#endif /* SRC_EVENT_HANDLER_H_ */
