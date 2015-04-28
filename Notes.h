#ifndef Notes
#define Notes
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------			This is our Notes.header						---------
---------															---------
---------			We will store functions related					---------
---------			to generating and playing notes here.			---------
---------															---------
---------			The two accessible (external) methods 			---------
---------			should be the DAC_Init(), for setting up 		---------
---------			DAC at the start, and PlayNote() which			---------
---------			will take a string and a octave into			---------
---------			the function. i.e.								---------
---------															---------
---------			PlayNote("A", 4);								---------
---------			This will play a 440Hz sine wave				---------
---------															---------
---------															---------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------

*/


#include <string.h>



/*    Definitions    */
#define  	WAVE_RESOLUTION    256   	// Our 256bit sine wave resolution
#define    	MAX_VOLUME        16    	// 16 different volumes
#define 	SINE_OFFSET     128 		// DC offset for sin wave
#define 	DEFAULT_OCTAVE	4


/*    Global Variables        */
unsigned  int    data    theta = 0;		


/* Variable for moving through the 8-bit sine wave */
unsigned char    data     volume = 	1; 	/* Volume 0-15. 0=> mute, 15=> max */
unsigned char    data    octave = 	4; 	/* Set inital octave to 4 */

/* Tones and their frequencies 		C		C#		D		Eb		E		F		F#		G		G#		A		Bb		B	*/
unsigned short	 code	tone[]	=	{262,	277,	294,	311,	330,	349,	370,	392,	415,	440,	466,	494};

const char    code    sin[] = { 
                                    /* DAC voltages for 8-bit, 16 volume sine wave */
                                    /* ------------------------------------------------------------------------ */
                                    
                                    #include "sine_8_bits.csv"    /* 256 piece sine wave */
                                };
								
								
const unsigned char	code delay_HB[] = {
																		#include "delay_HB.csv"
};

const unsigned char	code delay_LB[] = {
																		#include "delay_LB.csv"
};

								
volatile unsigned short Dtheta;		//Our Dtheta variable does...

void set_Tone(unsigned short);
unsigned short octave_Adjust(unsigned char, unsigned char);
void DAC_Init();
void Timer_Init();
void Voltage_Reference_Init();
void DAC_Sine_Wave();
void Set_Volume(unsigned char);

/*		Voltage_Reference_Init	*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Voltage_Reference_Init

        Description:      Initialise voltage references (Needed for DAC)

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Voltage_Reference_Init()
{
    SFRPAGE   = ADC0_PAGE;
    REF0CN    = 0x02;
}


/*		Oscillator_Init			*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Oscillator_Init

        Description:      Initialise the system Clock at a faster rate  (Needed for DAC)

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Oscillator_Init()
{
    SFRPAGE   = CONFIG_PAGE;
    OSCICN    = 0x82;
}


/*		Timer_Init				*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer_Init

        Description:      Initialise timer ports and registers

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer_Init()
{
    SFRPAGE   = TMR2_PAGE;
    TMR2CN    = 0x04;
    TMR2CF    = 0x0A;
    RCAP2L    = 0x45;
    RCAP2H    = 0xFF;
}


/*		DAC_Init				*/
 /*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Init

        Description:      Initialise DAC0. 

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void DAC_Init()
{
	//We want this function to set up our DAC for general use. We will use the SetDAC() method for specifically readying the DAC for playing notes
	SFRPAGE   = DAC0_PAGE;
    DAC0CN    = 0x84; 
}


/*		Interrupts_Init			*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Interrupts_Init

        Description:      Initialise interrupts

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Interrupts_Init()
{
    IE        = 0xA0;  // Global enable interrupt + timer 2 interrupt
}


/*		Timer2_ISR				*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer2_ISR

        Description:      

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer2_ISR (void) interrupt 5
{
      DAC_Sine_Wave();
    
    TF2 = 0;        // Reset Interrupt

}


/*		DAC_Sine_Wave			*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Sine_Wave

        Description:      Produces a simple sine wave based on the volume, theta, and Dtheta values.

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
/* Run through sine wave */
void DAC_Sine_Wave(void){
    unsigned char i = (unsigned char)((theta&0xFF00)>>8);
    DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*    Update the voltage in the DAC    */
    theta = theta + Dtheta;    /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
}


/*		Set_Volume				*/
	

/*--------------------------------------------------------------------------------------------------------------------
        Function:         Set_Volume

        Description:      Sets the volume of the wave

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
	void Set_Volume(unsigned char i){
    if(i>MAX_VOLUME){    /* Adjust volume if it's value is too large */
        i = MAX_VOLUME;
    }
    volume = i;
}


unsigned short octave_Adjust(unsigned char OCT, unsigned char piano_key_select)
{		
		char move = OCT - DEFAULT_OCTAVE;
		unsigned short altered_FREQ = tone[piano_key_select];
		
		if(move>0){
			altered_FREQ = altered_FREQ<<(move); /* multiply by 2^move */
		}else if(move<0){
			move = -move; /* make move positive */
			altered_FREQ = altered_FREQ>>(move); /* divide by 2^move */
		}
		
    return(altered_FREQ);
}

void set_Tone(unsigned short i)
{
    Dtheta = i;
}



void delay(unsigned char delay_len){ /* a millisecond delay - caution CPU can do nothing else while running this delay*/
	if(delay_len==0){	/* Just to double check that we haven't called a delay which doesn't exist */
		delay_len++;
	}
	delay_len--;	/* Decrement delay_len so it points to the correct array pointer to value */
	if(delay_len>254){/* there are only 255 elements to array goes from  0 - to 254 */
		delay_len = 254;
	}
	TL1 = delay_LB[delay_len];	/* Set lowbyte 										*/
  TH1 = delay_HB[delay_len];	/* Set highbyte 									*/
	TF1 = 0;											/* Clear Timer1 Flag 							*/
	TR1 = 1; 										/* Turn on Timer1 								*/
	while(~TF1);								/* wait till timer flag is set 		*/
	TR1 = 0; 										/* Turn off Timer1 								*/
}


#endif