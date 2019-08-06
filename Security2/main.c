#include"uart.h"
#include"external_eeprom.h"
#include"string.h"
#include"timer0.h"
#include"timer0.h"
#include "string.h"
#include "motor.h"
#include "Buzzer.h"
#define NUMBER_OF_OVERFLOWS_PER_TEN_SECOND 40

uint8 g_tick = 0;
uint8 flag = 0;
uint8 Wait_Flag =0;

void fun(void){
    g_tick++;
	if(g_tick == NUMBER_OF_OVERFLOWS_PER_TEN_SECOND)
	{
		if(flag==0)
		{
			Close_Door();
			flag=1;
		}
		else
		{
			flag=0;
			Wait_Flag=1;
		}
		g_tick=0;
	}
}

void Door_Opened(void){

	Open_Door();
	Timer0_WakeUp(F_CPU_1024);
	while(Wait_Flag==0){}
	Timer0_Sleep();
	Stop_Motor();
	Wait_Flag=0;

}

void Buzzer_Alarm(void){
	Buzzer_On();
	flag=1;
	Timer0_WakeUp(F_CPU_1024);
		while(Wait_Flag==0){}
		Timer0_Sleep();
		Buzzer_Off();
		Wait_Flag=0;
}

void Recieve_Password(void){
	uint8 command[20];
	UART_receiveString(command);
	for(uint16 i=0;i<5;i++)
	{
		_delay_ms(10);
		EEPROM_writeByte(0x0311+i,command[i]);
	}
	_delay_ms(10);
	EEPROM_writeByte(0x0320,0x01);
}
void Send_Password(void){
	uint8 stored_password[20];
	for(uint16 i=0;i<5;i++)
	{
		EEPROM_readByte(0x0311+i, &stored_password[i]);
		_delay_ms(10);
	}
	stored_password[5]='\0';
	UART_sendString(stored_password);
	UART_sendString("#");
}
void Check_Stored_Password_In_External_Memory(void){
	uint8 flag;
	EEPROM_readByte(0x0320,&flag);
		_delay_ms(10);
	    if(flag==0x01){
	    	UART_sendByte(1);
	    }
	    else
	    {
	    	UART_sendByte(0);
	    }
}
int main (void){
uint8 command[20];
Timer_ConfigType  Configuration={
		    		.clock=F_CPU_1024,
					.interrupt=COMPARE_MODE,
					.mode=CTC,
					.Modes_Config.PWM_Enable=Disable,
					.Modes_Config.Compare_Value=250,
					.Modes_Config.CTC_Mode=NORMALOPERATION
		    };

    Motor_Init();
    Buzzer_Init();
	EEPROM_init();
	UART_init();
	Timer0_init(&Configuration);
	SREG  |= (1<<7);           // enable global interrupts in MC.
	Timer0_setCallBack(fun);

	Check_Stored_Password_In_External_Memory();

	while(1)
	{
		      UART_receiveString(command);

					if(strcmp(command,"a") == 0){ //motor

						Door_Opened();

					}else if(strcmp(command,"$") == 0){ //buzzer

						Buzzer_Alarm();

					}else if(strcmp(command,"&") == 0){ //stored_password

						Recieve_Password();

					}else if(strcmp(command,"*") == 0){ //send_password

						Send_Password();
					}
	}


	return 0;
}
