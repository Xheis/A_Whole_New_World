/*_____________________________________________________________________________________________________________________

        Assignment:       Elec2700
        Module:         Assignment 2
        Author:         Chris Caelli & Tim Farnworth
        Date:           20/4/15

        Description:
        This program will emulate a MIDI keyboard and produce excellent music (probably)


        Notes:          
		Basic Structure is being implemented. The code is limited to strict C.
		Current c51.exe compiler compiles the program, however gives 5 warnings of "Uncalled Functions".
		This is okay as we're implimenting stub programs.
		
        Revisions:		v0.1 Alpha

_____________________________________________________________________________________________________________________*/
#include "c8051F120.h"	//<SiLABS/C8051F120.h>
#include "Assign2.h"
#include "Notes.h"
#include "Keyboard.h"
//#include "LCD.h"


/* Variables */
char gameState = 1;		/* Used for switching to and from the volume and play states	*/

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

	while(1)
	{	
		Check_State_Change();
		switch (gameState) /* Decide which state we are in */
		{
			case 0: /* Volume Menu */
				//Code here
				LD7 = 0;
				delay(250);
				delay(250);
				LD6 = ~LD6;
				break;
			case 1: /* Play Menu */
				PORT1_TO_PLAY_TONE();
				break;
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------
        Function:         General_Init

        Description:      Initialise ports, watchdog....

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void General_Init()
{
	WDTCN = 0xde;
	WDTCN = 0xad;
  SFRPAGE = CONFIG_PAGE;
	P2MDOUT = 0xff;		// Need to make pushpull outputs to drive LEDs properly
	XBR2 = 0x40;
}


/*--------------------------------------------------------------------------------------------------------------------
        Function:         Check_State_Change

        Description:      Initialise ports, watchdog....

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Check_State_Change()
{
	if (MPB==0) /* Negative logic */
	{
		delay(25); //Calling a delay for 25ms
		if (MPB==0)
		{
			while (MPB==0);
			gameState = (gameState+1)%2;	/* This will take the current state, add 1, and return the remainder. i.e. toggle between 1 and 0.*/
			//this won't work without the delay pls.
		}
	}
}



