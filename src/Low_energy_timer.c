/*
 * Low_energy_timer.h
 *
 *  Created on: 30-Jan-2019
 *      Author: AMOGH
 */

/* Headers */
#include "em_cmu.h"
#include "gpio.h"
#include "em_letimer.h"
#include "Low_energy_timer.h"
#include "em_core.h"
#include "Event_handler.h"

/* Macros */

#define TIMER_S     (3)              // Select the time in seconds after which the interrupts should occur
#define MAX_TICKS   (65536)          // Letimer0 is a 16 bit timer, therefore maximum 65536 ticks are possible
#define MAX_TIME1   (4)              // Max time that can be counted by the timer in high frequency clock mode with a prescaler of 2
#define MAX_TIME2   (65.536)         // Max time that can be counted by the timer in ultra low frequency clock mode

uint32_t Ticks_to_be_loaded;         // ticks to be loaded in the comp1 register
//uint32_t div_freq;

//struct Possible_events event_set;

/*******************************************************************************
* Function Name: leTimer_config
********************************************************************************
*
* Summary:
*  This function enables the user to configure the letimer
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

void leTimer_config(void) // Init function for LETIMER
{
#if (SLEEP_MODE == 0)
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);     // Enable the Oscillator
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);  // Select clock source for LFA
    CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_2);    // Prescale by 4
#endif



#if (SLEEP_MODE == 1)

	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);      // Enable the Oscillator
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);   // Select clock source for LFA
	CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_2);     // Prescale by 4

#endif

#if (SLEEP_MODE == 2)

	CMU_OscillatorEnable(cmuOsc_LFXO, true, true); // Enable the Oscillator
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO); // Select clock source for LFA
	CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_2); // Prescale by 4
#endif

#if (SLEEP_MODE == 3)

	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true); // Enable the Oscillator
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO); // Select clock source for LFA


#endif


CMU_ClockEnable(cmuClock_LETIMER0, true);// Enable the required clock
LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT; //    REFERENCE

//letimerInit.enable = 0;

letimerInit.comp0Top = true;  // Autoreload the value in Comp 0
LETIMER_Init(LETIMER0, &letimerInit); // Initialize letimer0




#if (SLEEP_MODE == 0)
   // div_freq = 1 / 61.035 ;
    Ticks_to_be_loaded = (TIMER_S * MAX_TICKS)/ MAX_TIME1 ;  // (ticks to be loaded = timer after which interrupt should occur * max ticks)/ Maximum time delay that can be obtained in the given mode and frequency and prescaler
	LETIMER_CompareSet(LETIMER0, 0, Ticks_to_be_loaded); //Set values for comp0 and comp1

#endif

#if (SLEEP_MODE == 1)
	 // div_freq =  1 / 61.035;
	  Ticks_to_be_loaded = (TIMER_S * MAX_TICKS)/ MAX_TIME1 ;
	  LETIMER_CompareSet(LETIMER0, 0, Ticks_to_be_loaded); //Set values for comp0 and comp1

#endif

#if (SLEEP_MODE == 2)
	 // div_freq = 1 / 61.035 ;
	  Ticks_to_be_loaded = (TIMER_S * MAX_TICKS)/ MAX_TIME1 ;
	  LETIMER_CompareSet(LETIMER0, 0, Ticks_to_be_loaded); //Set values for comp0 and comp1
#endif


#if (SLEEP_MODE == 3)
	// div_freq = 1 / 1000 ;
	 Ticks_to_be_loaded = (TIMER_S * MAX_TICKS)/ MAX_TIME2 ;
	 LETIMER_CompareSet(LETIMER0, 0, Ticks_to_be_loaded); //Set values for comp0 and comp1
#endif


LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);  // Enable underflow interrupt
LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP1);// Enable COMP1 Interrupts


NVIC_EnableIRQ(LETIMER0_IRQn); //Enable LETIMER Interrupts in NVIC

}


/*******************************************************************************
* Function Name: LETIMER0_IRQHandler
********************************************************************************
*
* Summary:
*  This function enables the user to configure the letimer
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

void LETIMER0_IRQHandler(void)
{

   CORE_DECLARE_IRQ_STATE;                       // Start the critical section
   CORE_ENTER_CRITICAL();
   uint32_t reason = LETIMER_IntGet(LETIMER0);   // Get the current count value of the register and store it into variable

   if(reason & LETIMER_IF_UF)           // Check for Underflow
   {
	   event_set.timer_UF =1;           // Create an event on reaching the underflow
   event_set.event_null = 0;

	   gecko_external_signal(UF_FLAG);
   }

   LETIMER_IntClear(LETIMER0, reason);  // Clear pending interrupts
   CORE_EXIT_CRITICAL();           // End the critical section
}





