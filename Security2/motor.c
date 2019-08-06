
#include"motor.h"


void Motor_Init()
{
	DDRA = 0x03;
}

void Open_Door()
{
	PORTA = 0x01;
}

void Close_Door()
{
	PORTA = 0x02;
}

void Stop_Motor()
{
	PORTA = 0x00;
}
