/*
 * Low_energy_sleep.c
 *
 *  Created on: 30-Jan-2019
 *      Author: AMOGH
 */

#include "em_cmu.h"
#include "em_letimer.h"
#include "gpio.h"
#include "sleep.h"
#include "Low_energy_sleep.h"

/*******************************************************************************
* Function Name: sleep_config
********************************************************************************
*
* Summary:
*  This function enables the user to enable sleep in the Microcontroller
*
* Parameters:
*  None
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Reference:
*  Self
*******************************************************************************/

void sleep_config()
{
	const SLEEP_Init_t init = {0};  // Populate the structure with a 0 value
	SLEEP_InitEx(&init);
}




