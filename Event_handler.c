/*
 * Event_handler.c
 *
 *  Created on: Apr 27, 2019
 *      Author: amosh
 */

#include "src/Event_handler.h"
#include "src/I2C.h"


struct Possible_events event_set = {0};

//uint32_t div1;
//uint32_t mode_count_load ;

enum temperature_I2C_states{
	Power_off,
	Device_correct,
	Status_read,
	Application_uploaded,
	READ_MEAS_MODE_VAL,
    Measurement_mode_configured,
	display_co2,
};

enum temperature_I2C_states current_state = Power_off;
enum temperature_I2C_states next_state;

void scheduler(void){
	switch (current_state) {             // Switch for current state
	case Power_off:                     // Case 1, the board is in power off state
	{
		event_set.timer_UF = 0;     // Clear the event flag
		event_set.event_null = 1;
		hardware_id_check();
		next_state = Device_correct; // Next state
	}
	break;

	case Device_correct:        // Case 2, the board is waiting for power up

		if(event_set.hardware_id_pass == 1 )
		{
			event_set.hardware_id_pass = 0;
			event_set.event_null = 1;
			status_read();
			next_state = Status_read; // define next state
		}
		break;

	case Status_read:
		if(event_set.sensor_status == 1)
		{
			event_set.sensor_status = 0;
			event_set.event_null = 1;
			write_application();
			next_state = Application_uploaded; // define next state
		}
		break;

	case Application_uploaded:
		if(event_set.application_upload == 1 )
		{
			event_set.application_upload = 0;
			event_set.event_null = 1;
			sensor_mode_set();
			next_state = READ_MEAS_MODE_VAL; // define next state
		}
		break;

	case READ_MEAS_MODE_VAL:
		LOG_INFO("Value is %d and %d", event_set.sensor_mode_set, event_set.timer_UF);

		if(event_set.sensor_mode_set == 1  || event_set.timer_UF == 1)
		{
			event_set.sensor_mode_set = 0;
			event_set.event_null = 1;
			read_meas_mode();
			next_state = Measurement_mode_configured; // define next state
		}
		//if failed
		break;

	case Measurement_mode_configured:
		if(event_set.meas_mode_data_read == 1 )
		{
			event_set.meas_mode_data_read = 0;
			event_set.event_null = 1;
			CO2_value_calculation();
			next_state = display_co2; // define next state
		}
		break;

	case display_co2:
		if(event_set.value_calculated == 1)
		{
			event_set.value_calculated = 0;
			event_set.event_null = 1;
			CO2_value_display();
			next_state = READ_MEAS_MODE_VAL; // define next state
		}
	}

	// Changing states
	if(current_state != next_state)           // Check if state transition has taken place
			{
				LOG_INFO("State transferred from %d to %d", current_state, next_state);
				current_state = next_state;           // Set the current state to next state
			}
}
