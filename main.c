#include"lcd.h"
#include"uart.h"
#include"keypad.h"
#include"string.h"
#include"timer0.h"

/*
 * Define number of overflow required by timer to reach 10 sec
 */
#define NUMBER_OF_OVERFLOWS_PER_TEN_SECOND 40
/*
 *
 */
uint8 g_tick = 0;       //counter for current overflow of timer
uint8 flag = 0;         // flag used to initiate door task
uint8 Wait_Flag =0;     // busy wait flag for delay task
uint8 New_Password[6];  // New password entered by user
uint8 Requested_Password[6];  //used to save password stored on EEPROM if there is

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description: Function passed to CallBackfunction used to open and close door for 10 sec for each task
 *  also used to initiate buzzer task for 10 sec
 */
void fun(void){
    g_tick++;
	if(g_tick == NUMBER_OF_OVERFLOWS_PER_TEN_SECOND)
	{
		if(flag==0)
		{
			LCD_displayStringRowColumn(0, 0, "Door Is Close");
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
/*
 * Description: used to handle Text on LCD in case
 * 1- enter password for first time
 * 2- reentering password for matching case
 * 3- requested to open door or change password
 */
void Handling(void) {

	LCD_displayStringRowColumn(0, 0, "Enter The New");
	LCD_displayStringRowColumn(1, 0, "password -----");

}
void ReHandling(void){
	LCD_displayStringRowColumn(0, 0, "ReEnter The New");
	LCD_displayStringRowColumn(1, 0, "password -----");
}
void Handling_Screen(void) {

	LCD_displayStringRowColumn(0, 0, "Enter The Old");
	LCD_displayStringRowColumn(1, 0, "password -----");
}

void ReHandling_Screen(void) {

	LCD_displayStringRowColumn(0, 0, "ReEnter The Old");
	LCD_displayStringRowColumn(1, 0, "password -----");
}

/*
 * Description: Function used to handling Password consist of 5 numbers entered by
 *  user in any case(change - new password- open door) from Keypad.
 */

void Writing_Five_Digits_Password(uint8 * statement){

	uint8 counter = 0;   //counter for numbers of password's digits
	uint8 key;           // key to get number entered by user
	while (counter < 5) {

			key = KeyPad_getPressedKey();
			if (key <= 9 && key >= 0) {
				LCD_displayStringRowColumn(1, (9 + counter), "*");
				statement[counter] = (0x30 + key);
				counter++;
			}
			statement[counter]='\0';
			_delay_ms(200);

		}
}

/*
 *  Description: function used to get password from user twice and check for matching
 *
 */

uint8 Enter_New_Password(uint8 *password) {

	uint8 First_Password[6];
	uint8 Second_Password[6];
	LCD_clearScreen();
	/*
	 * User Set the Password
	 */
	Handling();
	Writing_Five_Digits_Password(First_Password);
	/*
	 * User reSet the Password
	 */
	ReHandling();
	Writing_Five_Digits_Password(Second_Password);
    /*
     * check for matching
    */
	if (strcmp(First_Password,Second_Password) == 0)
	{
		strcpy(password,First_Password);
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*
 * Description:function used to send password using uart using
 * command "&" + Password to save it on EEPROM
 */
void Save_Password(void){

	UART_sendString("&#");
	UART_sendString(New_Password);
	UART_sendByte('#');

}

/*
 * Description: function use to send command "*" to get password saved on EEPROM
 */

void Request_Password(uint8 *password){

	uint8 Recieved_Password[20];
	UART_sendString("*#");
	UART_receiveString(Recieved_Password);
    strcpy(password,Recieved_Password);

}

/*
 * Description: Function use to Handling Buzzer Task for 10 sec
 */

void Buzzer(void){

	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Hramiiiiiiiiii");
	UART_sendString("$#");
	flag=1;
	Timer0_WakeUp(F_CPU_1024);       //Initiate Timer to count
	while(Wait_Flag==0){}            // Busy wait till toggling by Timer After Closing door
	Timer0_Sleep();                  //Stop Timer from Counting
	Wait_Flag=0;

}
/*
 * Description:Function used to check Stored Password with That Entered by user
 * To Change password and get two matching passwords from user
 */
void Change_Password(void){

		uint8 Stored_Password[6];
		uint8 repeated_counter=0;

	Request_Password(Requested_Password);

	LCD_clearScreen();

	do{
		if(repeated_counter!=0)
	    {
            ReHandling_Screen();
	    }
	    else
	    {
		    Handling_Screen();
	    }

		Writing_Five_Digits_Password(Stored_Password);

		repeated_counter++;

	}while(!((strcmp(Stored_Password,Requested_Password) == 0) || (repeated_counter>=3)));

	LCD_clearScreen();

	if((strcmp(Stored_Password,Requested_Password) == 0))
	{
		      while((Enter_New_Password(New_Password)==FALSE)){};
		       Save_Password();
	}
	else
	{

		Buzzer();

	}

}

/*
 * Description: Function used to initiate Open Door Task
 * by send command "a" Using uart to Handling it on Hardware
 */
void Door_Opened(void){

	UART_sendString("a#");
	LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Door Is Open");
	Timer0_WakeUp(F_CPU_1024);
	while(Wait_Flag==0){}
	Timer0_Sleep();
	Wait_Flag=0;

}
/*
 * Description:Function used to check Stored Password with That Entered by user
 * To open Door
 */
void Open_Door(void){

	    uint8 Stored_Password[6];
	    uint8 repeated_counter=0;

		Request_Password(Requested_Password);

		LCD_clearScreen();

		do{
			if(repeated_counter!=0)
				    {
			            ReHandling_Screen();
				    }
				    else
				    {
					    Handling_Screen();
				    }

			Writing_Five_Digits_Password(Stored_Password);
			repeated_counter++;

		}while(!((strcmp(Stored_Password,Requested_Password) == 0) || (repeated_counter>=3)));

		LCD_clearScreen();

		if( (strcmp(Stored_Password,Requested_Password) == 0))
			{

            Door_Opened();

			}
			else
			{

			Buzzer();

			}
}

/*
 * Description: Function to Handling Task of get user command
 * user enter '-' for open door task or for '+' request to change password
 */
uint8 Start (void){

	uint8 key;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+ ChangePassword");
	LCD_displayStringRowColumn(1, 0, "- OpenDoor");

	key=KeyPad_getPressedKey();
	return key;

}

/*
 * Description: Function used to Check if There is password stored or its first time to
 *              program the Application ,Specially use on Power on and reset cases
 */

void Check_Stored_Password_In_External_Memory(void){
    uint8 flag;
    flag=UART_recieveByte();
    	if(flag==1){

    	}else{

    		// Polling on Enter Password Till User Enter 2 Matched Passwords

    		 while(Enter_New_Password(New_Password)==FALSE){};

    		// Save Password On EPPROM

    		    Save_Password();
    	}
}
int main(void) {

	// Initiate Container to Config the Timer0 Module

	 Timer_ConfigType  Configuration={
		    		.clock=F_CPU_1024,
					.interrupt=COMPARE_MODE,
					.mode=CTC,
					.Modes_Config.PWM_Enable=Disable,
					.Modes_Config.Compare_Value=250,
					.Modes_Config.CTC_Mode=NORMALOPERATION
		    };

	 /*
	  * Initialize Modules
	  */

	uint8 Pressed_Key;
	UART_init();
	LCD_init();
	Timer0_init(&Configuration);
	SREG  |= (1<<7);           // enable global interrupts in MC.
	Timer0_setCallBack(fun);

    // Check Password Availability

	Check_Stored_Password_In_External_Memory();

	while(1)
	{
    //Wait till user enter '-' for open door task or for '+' request to change password

	Pressed_Key=Start();

		if(Pressed_Key=='+')
		    {
		    	 Change_Password();
		    }
		else if(Pressed_Key=='-')
		    {
		    	Open_Door();
		    }
		else{

		    }
	}

	return 0;
}
