/*
 * timer0.h
 *
 *  Created on: Jul 14, 2019
 *      Author: Mhmoud Ibrahim
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef uint8 Timer_ValueType;
typedef enum
{
	Success,Failed
}Task;
typedef enum
{
	NORMAL=0x00,PWM_PHASE_CORRECT=0x08,CTC=0x40,FAST_PWM=0x48
}Timer_ModeType;

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer_Clock;

typedef enum
{
	Enable,Disable
}PWM_Mode_Enable;

typedef enum
{
	NORMALOPERATION=0b00
	,Reserved=0b01
	,TOGGLE_OC0=0b01
	,NON_INVERTING_MODE=0b10
	,CLEAR_OC0=0b10
	,INVERTING_MODE=0b11
	,SET_OC0=0b11
}PWM_Mode_ConfigType;

typedef enum
{
  NONE,NORMAL_MODE,COMPARE_MODE
}Timer_Interrupt;

typedef struct
{
  PWM_Mode_Enable PWM_Enable;
  PWM_Mode_ConfigType CTC_Mode;
  Timer_ValueType  Compare_Value;
}Timer_Compare_Mode_ConfigType;


typedef struct
{
	Timer_Clock clock;
	Timer_ModeType mode;
	Timer_Compare_Mode_ConfigType Modes_Config;
	Timer_Interrupt interrupt;
}Timer_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void Timer0_init(const Timer_ConfigType * Config_Ptr);

void Timer0_Sleep(void);

void Timer0_WakeUp(Timer_Clock clockSelect);

Timer_ValueType Timer0_getValue(void);

Task Timer0_set_PWM_DutyCycle(uint8 set_duty_cycle);

void Timer0_setCallBack(void(*a_ptr)(void));    //DONE

void Timer0_clearTimer0Value(void);  //done

void Timer0_DeInit(void);   //done
#endif /* TIMER0_H_ */
