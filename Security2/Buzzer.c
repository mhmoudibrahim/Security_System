/*
 * Buzzer.c
 *
 *  Created on: Jul 17, 2019
 *      Author: lenovo
 */


#include"Buzzer.h"

void Buzzer_Init(void){

	SET_BIT(DDRB,PB0);
}
void Buzzer_On(void){

	SET_BIT(PORTB,PB0);

}
void Buzzer_Off(void){

	CLEAR_BIT(PORTB,PB0);
}
