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
#include "gpio.h"
#include "mesh_lib.h"

// soft timer flags
#define DISPLAY_REFRESH 0x05
#define LOG_REFRESH 0x06
#define TIMER_ID_FACTORY_RESET 0x07
#define TIMER_ID_RESTART 0x08
#define TIMER_ID_FRIEND_FIND 0x09

// external event flags
#define PUSHBUTTON_FLAG 0x01
#define FLAME_SENSOR_FLAG 0x02

/* MACROS FOR DATA SENT VIA MODELS */
#define PB0_STOP_ALERT          (0x01)        // LEVEL model
#define VIBRATION_ALERT         (0x0A)        // LEVEL model
#define LIGHT_CONTROL_ON        (0x1B)        // LEVEL model
#define LIGHT_CONTROL_OFF       (0x2B)        // LEVEL model
#define GAS_ALERT               (0x0C)        // LEVEL model
#define FIRE_ALERT              (0x0D)        // LEVEL model
#define NOISE_ALERT             (0x0E)        // LEVEL model
#define HUMIDITY_ALERT          (0x0F)        // LEVEL model


/// number of active Bluetooth connections
/// as shown in Silicon Labs switch example
static uint8 num_connections = 0;

uint32_t tickCount;
uint8_t trid;

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

#endif /* SRC_SCHEDULER_H_ */
