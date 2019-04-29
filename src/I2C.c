/*
 * I2C.C
 *
 *  Created on: 05-Feb-2019
 *      Author: AMOGH
 */
/***************************************************************************************
 *                          HEADERS                                                     *
 ***************************************************************************************/
#include "I2C.h"
#include "log.h"
#include <stdbool.h>

/***************************************************************************************
 *                          GLOBAL VARIABLES                                            *
 ***************************************************************************************/
// Device address
uint8_t slave_address = 0xB5;
// Addresses of different registers in the sensor
uint8_t status_register = 0x00;
uint8_t application_register = 0xF4;
uint8_t mode_register = 0x01;
uint8_t hardware_id_register = 0x20;
// Data received in I2C transactions
uint8_t receive_data_store[2];
uint8_t receive_data_read_meas_mode;
uint8_t receive_status_register;
uint8_t receive_hardware_id_store;
// Data to be written in the CC811 register values
uint8_t mode_register_val = 0x10;
uint8_t data_register = 0x02;
// This variable contains the final value of CO2
uint16_t co2_data;




/***************************************************************************************
 *                          GLOBAL STRUCTURES                                           *
 ***************************************************************************************/

typedef struct
{
	bool application_write;
	bool meas_mode_data_set;
	bool sensor_mode;
	bool CO2_value_calculated;
	bool hardware_id_checked;
	bool status_read_app_valid;

}Gas_sensor;


/***************************************************************************************
 *                          GLOBAL STRUCTURE INSTANCES                                  *
 ***************************************************************************************/
Gas_sensor I2C_flag;
extern struct Possible_events event_set;
I2C_TransferReturn_TypeDef result;
I2C_TransferSeq_TypeDef sequence_write;








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
 * Reference: Self
 *
 *******************************************************************************/


void hardware_id_check()
{
	sequence_write_hardware_id.flags = I2C_FLAG_WRITE_READ;
	sequence_write_hardware_id.addr = slave_address;
	sequence_write_hardware_id.buf[0].data = &hardware_id_register;             // Store the data from the temperature sensor in a buffer

	sequence_write_hardware_id.buf[1].data = &receive_hardware_id_store;

	sequence_write_hardware_id.buf[0].len = 1;
	sequence_write_hardware_id.buf[1].len = 1;

	LOG_INFO("ID : %d", (sequence_write_hardware_id.buf[1].data));
	LOG_INFO("ID : %d", *(sequence_write_hardware_id.buf[1].data));
	I2C_flag.hardware_id_checked = 1;
	I2C_TransferInit(I2C0, &sequence_write_hardware_id);

	LOG_INFO("ID : %d", (sequence_write_hardware_id.buf[1].data));
	LOG_INFO("ID : %d", *(sequence_write_hardware_id.buf[1].data));

	if ( *(sequence_write_hardware_id.buf[1].data) == 129 )
	{
		LOG_INFO("ID matching ");
	}
	else
	{
		LOG_INFO("ERROR in ID");
	}
}

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
 * Reference: Self
 *
 *******************************************************************************/

void write_application()
{
	sequence_write_application.flags = I2C_FLAG_WRITE;
	sequence_write_application.addr = slave_address;
	sequence_write_application.buf[0].data = &application_register;             // Store the data from the temperature sensor in a buffer
	sequence_write_application.buf[0].len = 1;                     // 2 bytes of data stored in the buffer
	I2C_flag.application_write = 1;

	I2C_TransferInit(I2C0, &sequence_write_application);
}

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
 * Reference: Self
 *
 *******************************************************************************/

void sensor_mode_set(void)
{
	sequence_write_sensor_mode.flags = I2C_FLAG_WRITE_WRITE;
	sequence_write_sensor_mode.addr = slave_address;
	sequence_write_sensor_mode.buf[0].data = &mode_register;
	sequence_write_sensor_mode.buf[1].data = &mode_register_val;
	sequence_write_sensor_mode.buf[0].len = 1;
	sequence_write_sensor_mode.buf[1].len = 1;
	I2C_flag.sensor_mode = 1;

	I2C_TransferInit(I2C0, &sequence_write_sensor_mode);


}

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
 * Reference: Self
 *
 *******************************************************************************/

void read_meas_mode(void)
{
	sequence_write_read_sensor_mode.flags = I2C_FLAG_WRITE_READ;
	sequence_write_read_sensor_mode.addr = slave_address;
	sequence_write_read_sensor_mode.buf[0].data = &mode_register;
	sequence_write_read_sensor_mode.buf[1].data = &receive_data_read_meas_mode;
	sequence_write_read_sensor_mode.buf[0].len = 1;
	sequence_write_read_sensor_mode.buf[1].len = 1;
	I2C_flag.meas_mode_data_set = 1;

	I2C_TransferInit(I2C0, &sequence_write_read_sensor_mode);


}

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
 * Reference: Self
 *
 *******************************************************************************/

void CO2_value_calculation(void)
{
	sequence_write_calculation.flags = I2C_FLAG_WRITE_READ;
	sequence_write_calculation.addr = slave_address;
	sequence_write_calculation.buf[0].data = &data_register;
	sequence_write_calculation.buf[1].data = &receive_data_store;
	sequence_write_calculation.buf[0].len = 1;
	sequence_write_calculation.buf[1].len = 2;
	I2C_flag.CO2_value_calculated = 1;

	I2C_TransferInit(I2C0, &sequence_write_calculation);
}

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
 * Reference: Self
 *
 *******************************************************************************/

void CO2_value_display(void)
{
	co2_data = 0;

	co2_data = receive_data_store[0];
	co2_data = ((receive_data_store[0]) << 8);
	co2_data |= (receive_data_store[1]);
	LOG_INFO("%d", co2_data);

	if(co2_data > 700)
		gecko_external_signal(GAS_FLAG);
}

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
 * Reference: Self
 *
 *******************************************************************************/

void status_read(void)
{
	sequence_write_status_read.flags = I2C_FLAG_WRITE_READ;
	sequence_write_status_read.addr = slave_address;
	sequence_write_status_read.buf[0].data = &status_register;
	sequence_write_status_read.buf[1].data = &receive_status_register;
	sequence_write_status_read.buf[0].len = 1;
	sequence_write_status_read.buf[1].len = 1;
	I2C_flag.status_read_app_valid = 1;

	I2C_TransferInit(I2C0, &sequence_write_status_read);


}


/* Interrupt handler for I2C */
void I2C0_IRQHandler(void)
{
	I2C_TransferReturn_TypeDef status = I2C_Transfer(I2C0);
	if(status == i2cTransferDone)
	{
		if (I2C_flag.status_read_app_valid == 1)
		{
			I2C_flag.status_read_app_valid = 0;
			gecko_external_signal(APPLICATION_VALID);
		}

		if (I2C_flag.application_write == 1)
		{
			I2C_flag.application_write = 0;
			gecko_external_signal(APPLICATION_WRITE);
		}

		if (I2C_flag.sensor_mode == 1)
		{
			I2C_flag.sensor_mode = 0;
			gecko_external_signal(SENSOR_MODE);
		}

		if (I2C_flag.CO2_value_calculated == 1)
		{
			I2C_flag.CO2_value_calculated = 0;
			gecko_external_signal(C02_VALUE);
		}

		if (I2C_flag.hardware_id_checked == 1)
		{
			I2C_flag.hardware_id_checked = 0;
			gecko_external_signal(HARDWARE_ID_CHECKED);
		}

		if (I2C_flag.meas_mode_data_set == 1)
		{
			I2C_flag.meas_mode_data_set = 0;
			gecko_external_signal(MEASURE_MODE);
		}
	}

	else if(status != i2cTransferInProgress)
	{
		event_set.event_null = 0;
		LOG_INFO("I2C error ");
	}
}
