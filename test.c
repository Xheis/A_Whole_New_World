#include "c8051F120.h"	
#include "Assign2.h"
#include "Methods.c"
#include "LCD.c"


/* Variables */
unsigned char gameState = 1;		/* Used for switching to and from the volume and play states	*/
int debugcounter=0;


//--------------------------------------------------------------------------------------------------------------------
//                              Abstract & Menu Methods
//--------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------
        Function:         Main

        Description:      Main routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void main(void)
{

	General_Init();
	Oscillator_Init();
	Timer_Init();
	Voltage_Reference_Init();
	DAC_Init();
	Interrupts_Init();
	initLCD();
	
	printLCD("Hello World")
	
	while(1){
		
	}
}