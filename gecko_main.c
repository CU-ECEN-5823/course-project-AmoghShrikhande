/***************************************************************************//**
 * @Author: Amogh Shirkhande (This code was developed with the help of team members)
 *
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
//#include "ncp_gecko.h"

#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "src/ble_mesh_device_type.h"

#include "src/scheduler.h"
#include "src/display.h"
#include "src/log.h"
#include "src/gpio.h"

/* Variables required for project */
/* variables to store flash load data - needs to be global */
uint8_t flash_data[DISPLAY_DATA_LENGTH];
uint8_t uint_array[DISPLAY_DATA_LENGTH];
char char_array[DISPLAY_DATA_LENGTH];

/* Lights/LED0 state */
uint8_t lightState = 0;
uint8_t* lightStatePtr;

/* Character array to store display message */
char* displayString;
uint8_t* displayBuffer;

/* Buzzer and LED1 toggle count */
uint8_t toggleCount = 0;
uint8_t* toggleCountPtr;

/* Sensors activation flags */
uint8_t flameActivationFlag = 1;

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
		.sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
		.bluetooth.max_connections = MAX_CONNECTIONS,
		.bluetooth.max_advertisers = MAX_ADVERTISERS,
		.bluetooth.heap = bluetooth_stack_heap,
		.bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
		.bluetooth.sleep_clock_accuracy = 100,
		.bluetooth.linklayer_priorities = &linklayer_priorities,
		.gattdb = &bg_gattdb_data,
		.btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
		.pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
		.pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
		.pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
		.max_timers = 16,
};

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void mesh_native_bgapi_init(void);
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

/***************************************************************************//**
 * Initialize LPN functionality with configuration and friendship establishment.
 * (code taken from Silicon Labs switch example)
 ******************************************************************************/
void lpn_init(void)
{
	uint16 res;
	// Initialize LPN functionality.
	res = gecko_cmd_mesh_lpn_init()->result;
	if (res) {
		LOG_INFO("LPN init failed (0x%x)", res);
		return;
	}

	// Configure the lpn with following parameters:
	// - Minimum friend queue length = 2
	// - Poll timeout = 5 seconds
	res = gecko_cmd_mesh_lpn_configure(2, 1 * 1000)->result;
	if (res) {
		LOG_INFO("LPN conf failed (0x%x)", res);
		return;
	}

	LOG_INFO("trying to find friend...");
	res = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

	if (res != 0) {
		LOG_INFO("ret.code %x", res);
	}
}

/**
 * See light switch app.c file definition
 */
void gecko_bgapi_classes_init_server_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	//gecko_bgapi_class_mesh_generic_client_init();
	gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
	//gecko_bgapi_class_mesh_friend_init();
}


// Set name of the device
void set_device_name(bd_addr *pAddr)
{
	char name[20];
	uint16 res;

	sprintf(name, "5823LPN1 %02x:%02x", pAddr->addr[1], pAddr->addr[0]);

	// write device name to the GATT database
	res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
	if (res) {
		LOG_INFO("gecko_cmd_gatt_server_write_attribute_value() failed, code %x", res);
	}

	displayPrintf(DISPLAY_ROW_NAME, "%s", name);
	displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", pAddr->addr[0], pAddr->addr[1], pAddr->addr[2], pAddr->addr[3], pAddr->addr[4], pAddr->addr[5]);
	LOG_INFO("device name set");
}

void gecko_main_init()
{
	// Initialize device
	initMcu();
	// Initialize board
	initBoard();
	// Initialize application
	initApp();

	// Initialize display
	displayInit();

	// Initialize gpio
	gpioInit();
	// Minimize advertisement latency by allowing the advertiser to always
	// interrupt the scanner.
	linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

	gecko_stack_init(&config);

	// for server init and lpn init
	gecko_bgapi_classes_init_server_lpn();

	// Initialize coexistence interface. Parameters are  taken from HAL config.
	gecko_initCoexHAL();

}

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	switch (evt_id) {
		// BOOT ID
		case gecko_evt_system_boot_id:
			if (GPIO_PinInGet(PB0_PORT, PB0_PIN) == 0 || GPIO_PinInGet(PB1_PORT, PB1_PIN) == 0) {
				gecko_cmd_flash_ps_erase_all();
				gecko_cmd_hardware_set_soft_timer(32768*2, TIMER_ID_FACTORY_RESET, 1);
				displayPrintf(DISPLAY_ROW_ACTION, "Factory Reset");
				LOG_INFO("factory reset");
			} else {
				LOG_INFO("boot done");
				struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();
				set_device_name(&pAddr->address);
				gecko_cmd_mesh_node_init();

				// loading persistent data
				lightStatePtr = flash_mem_retrieve(LIGHTS_MEM_ID);
				lightState = *lightStatePtr;

				toggleCountPtr = flash_mem_retrieve(ALERT_MEM_ID);
				toggleCount = *toggleCountPtr;
				LOG_INFO("TOGGLE COUNT = %d", toggleCount);

				displayBuffer = flash_mem_retrieve(DISPLAY_MEM_ID);
				displayString = convertUint(displayBuffer);
				LOG_INFO("display message: %15s", displayString);
			}
			break;

		// NODE INITIALIZED ID
		case gecko_evt_mesh_node_initialized_id:
			LOG_INFO("in mesh node initialized");

			struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

			if (pData->provisioned) {
				LOG_INFO("node is provisioned");
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");

				// PERSISTENT DATA EXECUTION
				lightState ? gpioLed0SetOn() : gpioLed0SetOff();

				if(toggleCount != 0)
					gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);

				displayPrintf(DISPLAY_ROW_SENSOR, "%15s", displayString);
				// PERSISTENT DATA EXECUTION END

				mesh_lib_init(malloc,free,9);
				init_models();

				// set node as generic server
				gecko_cmd_mesh_generic_server_init();

				// do low power initialization
				LOG_INFO("LPN mode initialization");
				lpn_init();

				// enable gpio interrupts
				gpio_interrupt_start();
			} else {
				LOG_INFO("node is unprovisioned");
				displayPrintf(DISPLAY_ROW_ACTION, "Unprovisioned");
				gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   // enable ADV and GATT provisioning bearer
			}
			break;

		// NODE PROVISIONING STARTED ID
		case gecko_evt_mesh_node_provisioning_started_id:
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");
			LOG_INFO("provisioning started");
			break;

		// NODE PROVISIONED ID
		case gecko_evt_mesh_node_provisioned_id:
			LOG_INFO("node is provisioned");
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");

			mesh_lib_init(malloc,free,9);
			init_models();

			// set node as generic server
			gecko_cmd_mesh_generic_server_init();

			// do low power initialization
			LOG_INFO("LPN mode initialization");
			lpn_init();

			// enable gpio interrupts
			gpio_interrupt_start();
			break;

		// NODE PROVISIONING FAILED ID
		case gecko_evt_mesh_node_provisioning_failed_id:
			LOG_INFO("provisioning failed, code %x", evt->data.evt_mesh_node_provisioning_failed.result);
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioning Failed");
			gecko_cmd_hardware_set_soft_timer(32768*2, TIMER_ID_RESTART, 1);
			break;


		// SERVER STATE CHANGED ID
		case gecko_evt_mesh_generic_server_state_changed_id:
			mesh_lib_generic_server_event_handler(evt);
			LOG_INFO("Server state changed id");
			break;

		// CLIENT REQUEST ID
		case gecko_evt_mesh_generic_server_client_request_id:
			mesh_lib_generic_server_event_handler(evt);
			LOG_INFO("Client request received id");
			break;

		// HARDWARE SOFTTIMER ID
		case gecko_evt_hardware_soft_timer_id:
			switch (evt->data.evt_hardware_soft_timer.handle) {
				// do display update every 1 second to remove charge buildup
				case DISPLAY_REFRESH:
					displayUpdate();
					break;

				// for logger time stamp - resolution of 10 msec
				case LOG_REFRESH:
					tickCount = tickCount + 10;
					break;

				case TIMER_ID_FACTORY_RESET:
					// reset the device to finish factory reset
					gecko_cmd_system_reset(0);
					break;

				case TIMER_ID_RESTART:
					// restart timer expires, reset the device
					gecko_cmd_system_reset(0);
					break;

				case FLAME_TIMEOUT_FLAG:
					// do not attend flame interrupts for a set period
					flameActivationFlag = 1;
					break;

				// case to find friend after particular interval
				case TIMER_ID_FRIEND_FIND:
					LOG_INFO("trying to find friend...");
					uint16_t result;
					result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;
					if (result != 0) {
						LOG_INFO("ret.code %x", result);
					}
					break;

				// blink alert LED 1 and buzzer for 10 seconds
				case LPN1_ALERT:
					// if toggleCount is an even number
					if(toggleCount % 2)
					{
						toggleCount++;
						flash_mem_store(ALERT_MEM_ID, &toggleCount);
						GPIO_PinOutSet(ALARM_PORT, ALARM_PIN);
						gpioLed1SetOn();
					}
					else
					{
						toggleCount++;
						flash_mem_store(ALERT_MEM_ID, &toggleCount);
						GPIO_PinOutClear(ALARM_PORT, ALARM_PIN);
						gpioLed1SetOff();

						// stop alerts after 10 seconds
						if(toggleCount > 100)
						{
							// reset toggleCount
							toggleCount = 0;
							flash_mem_store(ALERT_MEM_ID, &toggleCount);
							gecko_cmd_hardware_set_soft_timer(0, LPN1_ALERT, 0);

							// remove display message
							displayPrintf(DISPLAY_ROW_SENSOR, "               ");

							// storing the cleared display in persistent data
							displayString = "               ";
							displayBuffer = convertString(displayString);
							flash_mem_store(DISPLAY_MEM_ID, displayBuffer);
						}
					}
					break;
			}
			break;

		// CONNECTION OPEN ID
		case gecko_evt_le_connection_opened_id:
			LOG_INFO("in connection opened id");
			displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
			num_connections++;
			// turn off lpn feature after GATT connection is opened
			gecko_cmd_mesh_lpn_deinit();
			displayPrintf(DISPLAY_ROW_LPN, "LPN off");
			break;

		// CONNECTION CLOSE ID
		case gecko_evt_le_connection_closed_id:
			/* Check if need to boot to dfu mode */
			if (boot_to_dfu) {
				/* Enter to DFU OTA mode */
				gecko_cmd_system_reset(2);
			}
			LOG_INFO("in connection closed id");
			if (num_connections > 0) {
				if (--num_connections == 0) {
					displayPrintf(DISPLAY_ROW_CONNECTION, " ");
					lpn_init();
				}
			}
			break;

		// FRIENDSHIP ESTABLISHED ID
		case gecko_evt_mesh_lpn_friendship_established_id:
			LOG_INFO("friendship established");
			displayPrintf(DISPLAY_ROW_LPN, "LPN");
			break;

		// FRIENDSHIP FAILED ID
		case gecko_evt_mesh_lpn_friendship_failed_id:
			LOG_INFO("friendship failed");
			displayPrintf(DISPLAY_ROW_LPN, "no friend");
			// try again in 2 seconds
			gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FRIEND_FIND, 1);
			break;

		// FRIENDSHIP TERMINATED ID
		case gecko_evt_mesh_lpn_friendship_terminated_id:
			LOG_INFO("friendship terminated");
			displayPrintf(DISPLAY_ROW_LPN, "friend lost");
			if (num_connections == 0) {
				// try again in 2 seconds
				gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FRIEND_FIND, 1);
			}
			break;

		// EXTERNAL SIGNAL ID
		case gecko_evt_system_external_signal_id:
			;
			struct mesh_generic_state current;
			struct mesh_generic_state target;
			uint16_t resp;

			current.kind = mesh_generic_request_level;
			target.kind = mesh_generic_request_level;

			/* Scheduler external events starts */
			if ((evt->data.evt_system_external_signal.extsignals & HARDWARE_ID_CHECKED) != 0) {
				event_set.hardware_id_pass = 1;
				event_set.event_null = 0;
				scheduler();
			}

			if ((evt->data.evt_system_external_signal.extsignals & APPLICATION_VALID) != 0) {
				event_set.sensor_status = 1;
				event_set.event_null = 0;
				scheduler();
			}

			if ((evt->data.evt_system_external_signal.extsignals & APPLICATION_WRITE) != 0) {
				event_set.application_upload = 1;
				event_set.event_null = 0;
				scheduler();
			}


			if (((evt->data.evt_system_external_signal.extsignals & SENSOR_MODE) ||(evt->data.evt_system_external_signal.extsignals & UF_FLAG)) != 0) {
				event_set.sensor_mode_set = 1;
				event_set.event_null = 0;
				scheduler();
			}

			if ((evt->data.evt_system_external_signal.extsignals & MEASURE_MODE) != 0) {
				event_set.meas_mode_data_read = 1;
				event_set.event_null = 0;
				scheduler();
			}

			if ((evt->data.evt_system_external_signal.extsignals & C02_VALUE) != 0) {
				event_set.value_calculated = 1;
				event_set.event_null = 0;
				scheduler();
			}
			/* Scheduler external events ends */


			// push button case
			if ((evt->data.evt_system_external_signal.extsignals & PUSHBUTTON_FLAG) != 0)
			{
				LOG_INFO("PB0 pressed");

				// stop alerts
				toggleCount = 101;
				flash_mem_store(ALERT_MEM_ID, &toggleCount);

				// server publish alert stop data
				current.level.level = PB0_STOP_ALERT;
				target.level.level = PB0_STOP_ALERT;

				// do server update
				resp = mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, &current, &target, 0);
				if (resp) {
					LOG_INFO("gecko_cmd_mesh_generic_server_update failed,code %x", resp);
				} else {
					LOG_INFO("update done");
				}

				// publish server state
				resp = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, current.kind);
				if (resp) {
					LOG_INFO("gecko_cmd_mesh_generic_server_publish failed,code %x", resp);
				} else {
					LOG_INFO("request sent");
				}
			}

			// flame sensor case
			if ((evt->data.evt_system_external_signal.extsignals & FLAME_SENSOR_FLAG) != 0)
			{
				if(flameActivationFlag) {
					flameActivationFlag = 0;

#if 0
					// disable flame sensor gpio here
#endif

					// attend flame sensor interrupt after 5 seconds
					gecko_cmd_hardware_set_soft_timer(1 * 32768, FLAME_TIMEOUT_FLAG, 1);

					LOG_INFO("Flame Sensor interrupt");

					// display message
					displayPrintf(DISPLAY_ROW_SENSOR, "  FIRE ALERT  ");
					displayString = "  FIRE ALERT  ";
					flash_mem_store(DISPLAY_MEM_ID, convertString(displayString));

					// start alerts
					toggleCount = 0;
					flash_mem_store(ALERT_MEM_ID, &toggleCount);
					gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);

					// server publish flame alert
					current.level.level = FIRE_ALERT;
					target.level.level = FIRE_ALERT;

					// do server update
					resp = mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, &current, &target, 0);
					if (resp) {
						LOG_INFO("gecko_cmd_mesh_generic_server_update failed,code %x", resp);
					} else {
						LOG_INFO("update done");
					}

					// publish server state
					resp = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, current.kind);
					if (resp) {
						LOG_INFO("gecko_cmd_mesh_generic_server_publish failed,code %x", resp);
					} else {
						LOG_INFO("request sent");
					}
				}
			}

			// gas flag
			if ((evt->data.evt_system_external_signal.extsignals & GAS_FLAG) != 0)
			{
				LOG_INFO("Gas Sensor interrupt");

				// display message
				displayPrintf(DISPLAY_ROW_SENSOR, "   GAS ALERT   ");
				displayString = "   GAS ALERT   ";
				flash_mem_store(DISPLAY_MEM_ID, convertString(displayString));

				// start alerts
				toggleCount = 0;
				flash_mem_store(ALERT_MEM_ID, &toggleCount);
				gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);

				// server publish flame alert
				current.level.level = GAS_ALERT;
				target.level.level = GAS_ALERT;

				// do server update
				resp = mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, &current, &target, 0);
				if (resp) {
					LOG_INFO("gecko_cmd_mesh_generic_server_update failed,code %x", resp);
				} else {
					LOG_INFO("update done");
				}

				// publish server state
				resp = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, current.kind);
				if (resp) {
					LOG_INFO("gecko_cmd_mesh_generic_server_publish failed,code %x", resp);
				} else {
					LOG_INFO("request sent");
				}
			}
			break;

#if 0
		case gecko_evt_gatt_server_user_write_request_id:
			if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
				/* Set flag to enter to OTA mode */
				boot_to_dfu = 1;
				/* Send response to Write Request */
				gecko_cmd_gatt_server_send_user_write_response(
						evt->data.evt_gatt_server_user_write_request.connection,
						gattdb_ota_control,
						bg_err_success);

				/* Close connection to enter to DFU OTA mode */
				gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
			}
			break;
#endif

		// NODE RESET ID
		case gecko_evt_mesh_node_reset_id:
			LOG_INFO("in mesh node reset id");
			// reset mesh node
			gecko_cmd_hardware_set_soft_timer(32768*2, TIMER_ID_RESTART, 1);
			break;

		default:
			break;
	}
}

/*
 * Generic server register handler
 * */
static void init_models(void)
{
	// for ONOFF model
	mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
			0,
			onoff_request,
			onoff_change);

	// for LEVEL model
	mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
			0,
			level_request,
			level_change);
}

// ONOFF request callback function
static void onoff_request(uint16_t model_id,
		uint16_t element_index,
		uint16_t client_addr,
		uint16_t server_addr,
		uint16_t appkey_index,
		const struct mesh_generic_request *request,
		uint32_t transition_ms,
		uint16_t delay_ms,
		uint8_t request_flags)
{
	// Lights control - ON //
	if(request->on_off == LIGHT_CONTROL_ON) {
		gpioLed0SetOn();

		// save lights state in flash
		lightState = 1;
		flash_mem_store(LIGHTS_MEM_ID, &lightState);
	}

	// Lights control - OFF //
	if(request->on_off == LIGHT_CONTROL_OFF) {
		gpioLed0SetOff();

		// save lights state in flash
		lightState = 0;
		flash_mem_store(LIGHTS_MEM_ID, &lightState);
	}
}

// ONOFF change callback function
static void onoff_change(uint16_t model_id,
		uint16_t element_index,
		const struct mesh_generic_state *current,
		const struct mesh_generic_state *target,
		uint32_t remaining_ms)
{
	LOG_INFO("ONOFF State Changed");
}

// LEVEL request callback function
static void level_request(uint16_t model_id,
		uint16_t element_index,
		uint16_t client_addr,
		uint16_t server_addr,
		uint16_t appkey_index,
		const struct mesh_generic_request *request,
		uint32_t transition_ms,
		uint16_t delay_ms,
		uint8_t request_flags)
{
	// stop alerts //
	if(request->level == PB0_STOP_ALERT) {
		toggleCount = 101;
		flash_mem_store(ALERT_MEM_ID, &toggleCount);
	}

	// Earthquake alert //
	if(request->level == VIBRATION_ALERT) {
		displayPrintf(DISPLAY_ROW_SENSOR, "  EARTHQUAKE  ");
		displayString = "  EARTHQUAKE  ";
		flash_mem_store(DISPLAY_MEM_ID, convertString(displayString));

		// start alerts with a timeout of 10 seconds
		toggleCount = 0;
		flash_mem_store(ALERT_MEM_ID, &toggleCount);
		gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);
	}

	// Noise alert //
	if(request->level == NOISE_ALERT) {
		displayPrintf(DISPLAY_ROW_SENSOR, "  NOISE ALERT  ");
		displayString = "  NOISE ALERT  ";
		flash_mem_store(DISPLAY_MEM_ID, convertString(displayString));

		// start alerts with a timeout of 10 seconds
		toggleCount = 0;
		flash_mem_store(ALERT_MEM_ID, &toggleCount);
		gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);
	}

	// Humidity alert //
	if(request->level == HUMIDITY_ALERT) {
		displayPrintf(DISPLAY_ROW_SENSOR, "HUMIDITY ALERT");
		displayString = "HUMIDITY ALERT";
		flash_mem_store(DISPLAY_MEM_ID, convertString(displayString));

		// start alerts with a timeout of 10 seconds
		toggleCount = 0;
		flash_mem_store(ALERT_MEM_ID, &toggleCount);
		gecko_cmd_hardware_set_soft_timer(3277, LPN1_ALERT, 0);
	}
}

// LEVEL change callback function
static void level_change(uint16_t model_id,
		uint16_t element_index,
		const struct mesh_generic_state *current,
		const struct mesh_generic_state *target,
		uint32_t remaining_ms)
{
	LOG_INFO("LEVEL State Changed");
}

/* PERSISTENT DATA FUNCTIONS */
/***************************************************************************//**
 * Persistent Data Flash Load function
 ******************************************************************************/
uint8_t* flash_mem_retrieve(uint8_t flashID) {
    uint16 resp;
    struct gecko_msg_flash_ps_load_rsp_t* flash_resp;
    // array to store actual data, display length taken because that will be longest

    switch (flashID) {
    	// loads toggleCount
        case ALERT_MEM_ID:
            flash_resp = gecko_cmd_flash_ps_load(ALERT_ADDR);
            flash_data[0] = flash_resp->value.data[0];
            break;

        // loads display message
        case DISPLAY_MEM_ID:
            flash_resp = gecko_cmd_flash_ps_load(DISPLAY_ADDR);
            for(int i=0; i<DISPLAY_DATA_LENGTH; i++)    {
                flash_data[i] = flash_resp->value.data[i];
            }
            break;

        // loads lights status
        case LIGHTS_MEM_ID:
            flash_resp = gecko_cmd_flash_ps_load(LIGHTS_ADDR);
            flash_data[0] = flash_resp->value.data[0];
            break;
    }

    resp = flash_resp->result;
    if(resp) {
        LOG_INFO("flash load failed,code %x", resp);
    } else {
//        LOG_INFO("flash load success");
    }

    return flash_data;
}

/***************************************************************************//**
 * Persistent Data Flash Store function
 ******************************************************************************/
void flash_mem_store(uint8_t flashID, uint8_t *dataPtr) {
    uint16 resp;

    switch (flashID) {
    	// store toggleCount
        case ALERT_MEM_ID:
            resp = gecko_cmd_flash_ps_save(ALERT_ADDR, ALERT_DATA_LENGTH, dataPtr)->result;
            break;

        // store display message
        case DISPLAY_MEM_ID:
            resp = gecko_cmd_flash_ps_save(DISPLAY_ADDR, DISPLAY_DATA_LENGTH, dataPtr)->result;
            break;

        // store lights state
        case LIGHTS_MEM_ID:
            resp = gecko_cmd_flash_ps_save(LIGHTS_ADDR, LIGHTS_DATA_LENGTH, dataPtr)->result;
            break;
    }

    if (resp) {
        LOG_INFO("flash store failed,code %x", resp);
    } else {
//        LOG_INFO("flash store success");
    }
}

// conversion of string to uint8 array
// required before storing display message in flash
uint8_t* convertString(char* str) {
//    LOG_INFO("strlen = %d", strlen(str));
    for(int i=0; i<strlen(str); i++){
        uint_array[i] = (uint8_t)str[i];

        // print out each character - to verify
//        LOG_INFO("%c - %d", str[i], uint_array[i]);
    }
    return uint_array;
}

// conversion of uint8 array to string
// required after loading display message from flash
char* convertUint(uint8_t* array) {
//    LOG_INFO("SIZEOF = %d", sizeof(array));
    for(int i=0; i<DISPLAY_DATA_LENGTH; i++){
        char_array[i] = (char)array[i];

        // print out each character - to verify
//        LOG_INFO("%c - %d", char_array[i], array[i]);
    }
    return char_array;
}
