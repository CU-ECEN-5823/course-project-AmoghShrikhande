/*
 * scheduler.h
 *
 *  Created on: Apr 4, 2019
 *      Author: amosh
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "native_gecko.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "gpio.h"
#include "mesh_lib.h"
#include "Low_energy_timer.h"
#include "Event_handler.h"

#if 0
#define ONE_SEC_TICKS			(32768)
#endif


// for persistent data
/* Flash IDs for Flash Store and Load functions */
#define ALERT_MEM_ID            (0x02)
#define DISPLAY_MEM_ID          (0x03)
#define LIGHTS_MEM_ID           (0x04)

/* Flash Save Keys */
#define ALERT_ADDR        		(0x4001)
#define DISPLAY_ADDR       		(0x4002)
#define LIGHTS_ADDR       		(0x4003)

/* Persistent data lengths */
#define ALERT_DATA_LENGTH        (1)
#define DISPLAY_DATA_LENGTH      (15)
#define LIGHTS_DATA_LENGTH       (1)

// soft timer flags
#define DISPLAY_REFRESH 0x05
#define LOG_REFRESH 0x06
#define TIMER_ID_FACTORY_RESET 0x07
#define TIMER_ID_RESTART 0x08
#define TIMER_ID_FRIEND_FIND 0x09
#define FLAME_TIMEOUT_FLAG 0x10
#define LPN1_ALERT 0x11

// external event flags
#define PUSHBUTTON_FLAG 		0x0400
#define FLAME_SENSOR_FLAG 		0x0002
#define GAS_FLAG				0x0200

// scheduler external event flags
#define HARDWARE_ID_CHECKED  	0x0004
#define APPLICATION_VALID		0x0008
#define APPLICATION_WRITE		0x0800
#define SENSOR_MODE				0x0020
#define MEASURE_MODE			0x0040
#define C02_VALUE				0x0080
#define UF_FLAG					0x0010

/* MACROS FOR DATA SENT VIA MODELS */
#define PB0_STOP_ALERT          (0x01)        // LEVEL model
#define VIBRATION_ALERT         (0x0A)        // LEVEL model
#define LIGHT_CONTROL_ON        (0x01)        // ON OFF model
#define LIGHT_CONTROL_OFF       (0x00)        // ON OFF model
#define GAS_ALERT               (0x0C)        // LEVEL model
#define FIRE_ALERT              (0x0D)        // LEVEL model
#define NOISE_ALERT             (0x0E)        // LEVEL model
#define HUMIDITY_ALERT          (0x0F)        // LEVEL model


/// number of active Bluetooth connections
/// as shown in Silicon Labs switch example
static uint8 num_connections = 0;

uint32_t tickCount;

//function declarations
void set_device_name(bd_addr *pAddr);
static void init_models(void);

static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
uint8_t request_flags);

static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
uint32_t remaining_ms);

static void level_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
uint8_t request_flags);

static void level_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
uint32_t remaining_ms);

// PERSISTENT DATA FUNCTION DECLARATIONS
uint8_t* flash_mem_retrieve(uint8_t flashID);
void flash_mem_store(uint8_t flashID, uint8_t *dataPtr);
uint8_t* convertString(char* str);
char* convertUint(uint8_t* array);


#endif /* SRC_SCHEDULER_H_ */
