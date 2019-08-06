/*
 * timer0.c
 *
 *  Created on: Jul 14, 2019
 *      Author: Mhmoud Ibrahim
 */
#include "timer0.h"
#define NULL_PTR ((void*)0)
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

//.................................................................................................//
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
		{
			/* Call the Call Back function in the application after each matching compare value */
			(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
}

//.................................................................................................//



//.................................................................................................//

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer0_init(const Timer_ConfigType * Config_Ptr){

		TCCR0 =(TCCR0 & 0xF8);
		TCCR0 =(TCCR0 & 0x7F) | (((Config_Ptr->Modes_Config.PWM_Enable) & 0x01)<< 7);
		TCCR0 = (TCCR0 & 0xCF) | (((Config_Ptr->Modes_Config.CTC_Mode) & 0x03)<< 4);
        TCCR0 = (TCCR0 & 0xB7) | (Config_Ptr->mode);
		TCNT0=0;
		OCR0=(Config_Ptr->Modes_Config.Compare_Value);
	    TIMSK|= (Config_Ptr->interrupt);

}

void Timer0_Sleep(void){

	TCCR0 =(TCCR0 & 0xF8) ;
	TCNT0=0;
}


void Timer0_WakeUp(Timer_Clock clockSelect){
	TCCR0 =(TCCR0 & 0xF8) | (clockSelect & 0x07);
}


/*
 * Description: Function to get current value .
 */
Timer_ValueType Timer0_get_Value(void){
	return TCNT0;
}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

/*
 * Description: Function to clear the Timer0 Value to start count from ZERO
 */
void Timer0_clearTimer0Value(void){

	TCNT0 = 0;
}

/*
 * Description: Function to disable the Timer0 to stop the TIMER0 Driver
 */
void Timer0_DeInit(void){

	/* Clear All Timer1 Registers */
		TCCR0 = 0;
		TCNT0 = 0;
		OCR0 = 0;
	    TIMSK=0;
}
