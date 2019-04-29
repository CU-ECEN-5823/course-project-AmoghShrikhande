/*
 * I2C.C
 *
 *  Created on: 05-Feb-2019
 *      Author: AMOGH
 */

#include "I2C.h"
#include "log.h"
#include <stdbool.h>

//#define slave_address 0xB5
//#define slave_address 0xB6

uint8_t slave_address = 0xB5;
extern struct Possible_events event_set;     // Creating instance for the structure

bool write, read,sensor_status_check, application_write, meas_mode_data_set, sensor_mode, CO2_value_calculated, hardware_id_checked, status_read_app_valid;


/*******************************************************************************
 * Function Name: i2cinit
 ********************************************************************************
 *
 * Summary:
 *  Initializes the I2C
 *
 * Parameters:
 *  Void
 *
 * Return:
 *  None.
 *
 * Reentrant:
 *  No.
 *
 * Reference: Self
 *
 *******************************************************************************/

void i2cinit()
{
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;  //create an instance by the name i2cInit

	i2cInit.sclPin = 10;                      //  Port C pin 10 selected as scl pin
	i2cInit.sdaPin = 11;                      //  Port C pin 11 selected as sda pin
	i2cInit.portLocationScl = 14;             //  Port location selected as 14 for scl
	i2cInit.portLocationSda = 16;             //  Port location selected as 16 for scl

	I2CSPM_Init(&i2cInit);                    // Turns on the clock for high frequency peripheral, Enable clock
	// Sends clock pulses to set slave in defined state
	// Calls another lower level I2C init function

	I2C_IntEnable(I2C0,I2C_IEN_RXDATAV | I2C_IEN_RXFULL | I2C_IEN_TXC );
	NVIC_EnableIRQ(I2C0_IRQn);
}

//uint8_t command = 0xE3;                    // Initialize the variables
uint8_t status_register = 0x00;
uint8_t application_register = 0xF4;
uint8_t mode_register = 0x01;
uint8_t mode_register_val = 0x10;

uint8_t hardware_id_register = 0x20;
//uint8_t mode_register_val = 0x18;

uint8_t data_register = 0x02;

uint8_t read_tempData;
uint8_t receive_data_store[2];
uint8_t receive_data_read_meas_mode;
uint8_t receive_status_register;
uint8_t receive_hardware_id_store;

//uint8_t read_tempData;
uint8_t gas_sensor_status_reg;

uint16_t temperature_data;
uint8_t temperature_degreecelcius;

I2C_TransferReturn_TypeDef result;

I2C_TransferSeq_TypeDef sequence_write;        // Create instance for write sequence


void hardware_id_check()
{
	sequence_write_hardware_id.flags = I2C_FLAG_WRITE_READ;
	sequence_write_hardware_id.addr = slave_address;
	sequence_write_hardware_id.buf[0].data = &hardware_id_register;             // Store the data from the temperature sensor in a buffer

	sequence_write_hardware_id.buf[1].data = &receive_hardware_id_store;
	//&receive_hardware_id_store = sequence_write_hardware_id.buf[1].data;
	                    // 2 bytes of data stored in the buffer
	sequence_write_hardware_id.buf[0].len = 1;
	sequence_write_hardware_id.buf[1].len = 1;

	LOG_INFO("ID : %d", (sequence_write_hardware_id.buf[1].data));
	LOG_INFO("ID : %d", *(sequence_write_hardware_id.buf[1].data));
	hardware_id_checked = 1;
	I2C_TransferInit(I2C0, &sequence_write_hardware_id);

	LOG_INFO("ID : %d", (sequence_write_hardware_id.buf[1].data));
	LOG_INFO("ID : %d", *(sequence_write_hardware_id.buf[1].data));

	if ( *(sequence_write_hardware_id.buf[1].data) == 129 )
	{
//		LOG_INFO("DEVICE ID matches: %d", *(sequence_write_hardware_id.buf[1].data));
//		LOG_INFO("hardware_id_checked>>> : %d", hardware_id_checked);
	}
	else
	{
		LOG_INFO("ERROR in ID");
	}
}

void write_application()
{
	sequence_write_application.flags = I2C_FLAG_WRITE;
	sequence_write_application.addr = slave_address;
	sequence_write_application.buf[0].data = &application_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_application.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	application_write = 1;

	I2C_TransferInit(I2C0, &sequence_write_application);
}

void sensor_mode_set(void)
{
	sequence_write_sensor_mode.flags = I2C_FLAG_WRITE_WRITE;
	sequence_write_sensor_mode.addr = slave_address;
	sequence_write_sensor_mode.buf[0].data = &mode_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_sensor_mode.buf[1].data = &mode_register_val;
	sequence_write_sensor_mode.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	sequence_write_sensor_mode.buf[1].len = 1;
	sensor_mode = 1;

	I2C_TransferInit(I2C0, &sequence_write_sensor_mode);

//	LOG_INFO("Measurement register address: %d" , *(sequence_write_sensor_mode.buf[0].data) );
//	LOG_INFO("Measurement register written mode: %d" , *(sequence_write_sensor_mode.buf[1].data) );
}

void read_meas_mode(void)
{
	sequence_write_read_sensor_mode.flags = I2C_FLAG_WRITE_READ;
	sequence_write_read_sensor_mode.addr = slave_address;
	sequence_write_read_sensor_mode.buf[0].data = &mode_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_read_sensor_mode.buf[1].data = &receive_data_read_meas_mode;
	sequence_write_read_sensor_mode.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	sequence_write_read_sensor_mode.buf[1].len = 1;
	meas_mode_data_set = 1;

	I2C_TransferInit(I2C0, &sequence_write_read_sensor_mode);

//	LOG_INFO("Measurement register read mode: %d" , *(sequence_write_sensor_mode.buf[1].data) );
}


void CO2_value_calculation(void)
{
	sequence_write_calculation.flags = I2C_FLAG_WRITE_READ;
	sequence_write_calculation.addr = slave_address;
	sequence_write_calculation.buf[0].data = &data_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_calculation.buf[1].data = &receive_data_store;
	sequence_write_calculation.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	sequence_write_calculation.buf[1].len = 2;
	CO2_value_calculated = 1;

	I2C_TransferInit(I2C0, &sequence_write_calculation);
}

void CO2_value_display(void)
{
	temperature_data = 0;
	//LOG_INFO("%d", receive_data_store);
	temperature_data = receive_data_store[0];
	temperature_data = ((receive_data_store[0]) << 8);         // Shift the MSB by 8 bits and store the MSB in higher byte
	temperature_data |= (receive_data_store[1]);
	LOG_INFO("%d", temperature_data);

	if(temperature_data > 700)
		gecko_external_signal(GAS_FLAG);
}

void status_read(void)
{
	sequence_write_status_read.flags = I2C_FLAG_WRITE_READ;
	sequence_write_status_read.addr = slave_address;
	sequence_write_status_read.buf[0].data = &status_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_status_read.buf[1].data = &receive_status_register;
	sequence_write_status_read.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	sequence_write_status_read.buf[1].len = 1;
	status_read_app_valid = 1;

	I2C_TransferInit(I2C0, &sequence_write_status_read);

//	LOG_INFO("STATUS_REG_VAL : %d", *(sequence_write_status_read.buf[1].data));
}

void I2C0_IRQHandler(void)               // Interrupt handler for I2C
{
	I2C_TransferReturn_TypeDef status = I2C_Transfer(I2C0);
	if(status == i2cTransferDone)         // check status
	{
//		LOG_INFO ("I2C Transfer Complete");

		if (status_read_app_valid == 1)
		{
//			event_set.sensor_status = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			status_read_app_valid = 0;

			gecko_external_signal(APPLICATION_VALID);
		}

		if (application_write == 1)
		{
//			event_set.application_upload = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			application_write = 0;

			gecko_external_signal(APPLICATION_WRITE);
		}

		if (sensor_mode == 1)
		{
//			event_set.sensor_mode_set = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			sensor_mode = 0;
			gecko_external_signal(SENSOR_MODE);
		}

		if (CO2_value_calculated == 1)
		{
//			event_set.value_calculated = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			CO2_value_calculated = 0;

			gecko_external_signal(C02_VALUE);
		}

		if (hardware_id_checked == 1)
		{
//			LOG_INFO("In Hardware ID check");
//			event_set.hardware_id_pass = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			hardware_id_checked = 0;

			gecko_external_signal(HARDWARE_ID_CHECKED);
		}

		if (meas_mode_data_set == 1)
		{
//			event_set.meas_mode_data_read = 1;             // Set the event when read transfer is done
//			event_set.event_null = 0;
			meas_mode_data_set = 0;

			gecko_external_signal(MEASURE_MODE);
		}
	}

	else if(status != i2cTransferInProgress)
	{
		event_set.event_null = 0;
		LOG_INFO("I2C error ");
	}
}
