/*
 * I2C.h
 *
 *  Created on: 05-Feb-2019
 *      Author: AMOGH
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_


#include "i2cspm.h"                                  // To override the default structure
#include "em_gpio.h"
#include "em_i2c.h"
#include "log.h"
#include "Event_handler.h"
#include "scheduler.h"


// Global instances to access the structures to set the write data on the I2C bus and read the data from temp sensors
I2C_TransferSeq_TypeDef sequence_write;
I2C_TransferSeq_TypeDef sequence_read;
I2C_TransferSeq_TypeDef sequence_write_initialization;
I2C_TransferSeq_TypeDef sequence_write_application;
I2C_TransferSeq_TypeDef sequence_write_sensor_mode;
I2C_TransferSeq_TypeDef sequence_write_hardware_id;
I2C_TransferSeq_TypeDef sequence_write_read_sensor_mode;


I2C_TransferSeq_TypeDef sequence_write_calculation;

I2C_TransferSeq_TypeDef sequence_write_status_read;




// Function prototypes
void transfer();
void i2cinit();
void temperature_calculation();  // Function to calculate the temperature

void External_sensor_init();

void External_sensor_status_check();

void write_application();

void sensor_mode_set();

void CO2_value_calculation();

void hardware_id_check();

void status_read();

void read_meas_mode();

void CO2_value_display();

#endif /* SRC_I2C_H_ */
