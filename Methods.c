/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------				This is our Methods file that contains any 					---------
---------				computations we need																---------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

#include "Methods.h"

/*    Definitions    */
#define  	WAVE_RESOLUTION    	256   	// Our 256bit sine wave resolution
#define   MAX_VOLUME        	16    	// 16 different volumes
#define   MAX_FADER      			255    	// 256 different fading volumes
#define 	SINE_OFFSET     		128 		// DC offset for sin wave
#define 	DEFAULT_OCTAVE			7
#define 	DEFAULT_VOLUME			15			// Defualt volume. 0-15. 0=> mute
#define		NUM_NOTES						14
#define		NUM_NOTES		14


/*    Global Variables        */
volatile unsigned  short   	    theta[NUM_NOTES] = {0};		
volatile unsigned short d_theta[NUM_NOTES] = {0};		//Our d_theta variable does...
unsigned char 	data 		num_active_keys = 0; /* The number of keys which are currently being pressed */
unsigned char   data    volume = 	DEFAULT_VOLUME; 	/* Volume 0-15. 0=> mute, 15=> max */
unsigned char   data    octave = 	DEFAULT_OCTAVE; 	/* Set inital octave */
unsigned char 	data 		fader[NUM_NOTES] = {MAX_FADER};
unsigned char 	data 		fader_flag[NUM_NOTES] = {0};		/* this is for reseting the fader 0=>fader has been reset. 1=> fader is currently running */

/*	Tones and their frequencies		C		 D		E		 F	  G		 A		B	 	 C#	  D#   E   F#   G#   A#    B			*/
unsigned short	 code	tone[]	=	{262,	294, 330,	349, 392,	440, 494, 277, 311, 330, 370, 415, 466, 494};

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
    OSCICN    = 0x83;
}




/*		Timer_Init				*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer_Init

        Description:      Initialise timer ports and registers

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer_Init()
{
		
    SFRPAGE   = TIMER01_PAGE;					/* Initialize Timer 1 */
    TCON      = 0x40;
    TMOD      = 0x10;
    CKCON     = 0x02;	
    TL1       = delay_LB[0];
    TH1       = delay_HB[0];

	
    SFRPAGE   = TMR2_PAGE;	/* Timer 2 */
    TMR2CN    = 0x04;
    TMR2CF    = 0x0A;
    RCAP2L    = 0x8A;
    RCAP2H    = 0xFE;
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
		LD1 = 1;
    DAC_Multi_Sine_Wave();
    //DAC_Sine_Wave();  //  Working!
    TF2 = 0;        // Reset Interrupt

}



/*		DAC_Sine_Wave	--- DEPRECIATED		*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Sine_Wave

        Description:      Use to play a single note at a time

        Revisions:				DEPRECIATED. USED ONLY FOR DEBUGGING. IT SHOWS ALL THE CALCULATIONS WE USE AND WILL BE 
													LEFT AS AN EDUCATIONAL TOOL FOR FUTURE UPGRADES OR REVISIONS.

--------------------------------------------------------------------------------------------------------------------*/
	///* Run through sine wave */
	//void DAC_Sine_Wave(void){
	//		unsigned char i = (unsigned char)((theta[0]&0xFF00)>>8);
	//		DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*    Update the voltage in the DAC    */
	//    theta[0] = theta[0] + d_theta[0];    /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
	//}







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






/*		delay				*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         delay

        Description:      Creates a delay for any application e.g. debouncing

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void delay(unsigned short delay_len){
	unsigned char num_Run, i, GS;
	unsigned char exit_delay = 0;
	num_Run = (delay_len>>6)&0xFF; /* Number of times to run 6 bit delay */
	
	for (i = 0; i < num_Run; i++){
		exit_delay = delay_run(64);
		if(exit_delay){
			break;
		}
	}
	
	if(!exit_delay){ /* Only run remainder of the delay if exit has not been called */
		num_Run = delay_len&0x3F; /* Remainder of milliseconds to run */
		delay_run(num_Run); /* Run the remainder of the delay */
	}
}	





unsigned char delay_run(unsigned char delay_len){ /* a millisecond delay - caution CPU can do nothing else while running this delay*/
	unsigned char state = getState();
	if(delay_len==0){	/* Just to double check that we havent called a delay which doesnt exsist */
		delay_len++;
	}
	
	delay_len--;	/* Decrement delay_len so it points to the correct array pointer to value */
	if(delay_len>63){/* there are only 64 elements to array goes from  0 - to 63 */
		delay_len = 63;
	}
	
	TL1 = delay_LB[delay_len];	/* Set lowbyte 												*/
  TH1 = delay_HB[delay_len];	/* Set highbyte 											*/
	TF1 = 0;										/* Clear Timer1 Flag	 								*/
	TR1 = 1; 										/* Turn on Timer1 										*/
	
	while(~TF1){								/* wait till timer flag is set 				*/
															/* Checks to do during the delay	 		*/
		
		if(~MPB){									/* If Motherboard Push button is pressed */
			while(~MPB);						/* Wait till button is released */
			if(state){
					state = 0; 					/* Toggle state */
			}else{
					state = 1;
			}
			setState(state);
			return(1);							/* Exit delay 1=> stop delay*/			
		}
	}
	TR1 = 0; 										/* Turn off Timer1 								*/
	return(0);
}
		
unsigned char mirror_binary(unsigned char num){
	char i;
	unsigned char temp = 0;
	for(i=0; i<8; i++){
		temp += (((num>>i)&0x01)<<(7-i));
	}
	return(temp);
}



void PORT1_TO_PLAY_TONE(void){
	unsigned char alteredPort;
	unsigned char button_i;
	unsigned char i,j;
	unsigned short tone;
	unsigned char buttons_active = 0;
	UpdateLEDS(); /* Display the notes pushed on the keyboard */
	
	for(i = 1; i<8; i++){ 	/* Check buttons 1 to 7 inclusive */
		unsigned char tmp;
		tmp = P1;
		alteredPort = ~(tmp);
		button_i = (alteredPort>>i)&0x01; /* Move value to the right then increment by one then take the modulo to check whether button is active */
		/* button_i ACTIVE HIGH */
		buttons_active += button_i; /* Count the number of active notes */
		
		j = i-1; /* Normalize for pointing to an array */
		if(button_i){/* a key has been pressed */
			TR2 = 1;		/* Run timer2 if a button is set */
			
			
			if(~PB1){
				j = j+7; /* Adjust j so it will point to the sharp of the note */
			}
			
			tone = octave_Adjust(octave, j);
			set_Tone(tone, j);
			
		}else{ /* if button has not been pressed */
			set_Tone(0, j); /* Turn off note... This sets the d_theta to 0. We can therefore check the d_thetas for whether a note is being played */
			theta[j] = 0;		/* Reset theta position */
		}
	}
	num_active_keys = buttons_active; /* list the number of active buttons */
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




/*--------------------------------------------------------------------------------------------------------------------
        Function:         combined_Sine

        Description:      Performs all wave calculations and manipulations, and will output a multi-key, multi-tone 
													sine wave.

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
char	combined_Sine(void){
	unsigned char i,j; 
	short sine_combined = 0; /* The combined value of the notes */
	if(num_active_keys!=0){
		for(i = 0; i< NUM_NOTES; i++){
			j = (unsigned char)((theta[i]&0xFF00)>>8);
			sine_combined += sin[j];					/* Add fader control here */
			theta[i] = theta[i] + d_theta[i]; /* Increment theta */
		}
		return((char)(sine_combined/num_active_keys)); /* Return the new DAC Value */
	}
	else{
		return(0);
	}
}









/*--------------------------------------------------------------------------------------------------------------------
        Function:         set_Tone

        Description:      Set tone allows us to choose both an Octave and Note from our lookup table

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void set_Tone(unsigned short frequency, unsigned char note_select)
{
    d_theta[note_select] = frequency*4;

}








/*--------------------------------------------------------------------------------------------------------------------
        Function:         UpdateLEDs 

        Description:      Used to show in-play button presses

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void UpdateLEDS()
{
	P2 = ~P1; /* Invert Port1 and display it on P2 leds */
}




/*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Multi_Sine_Wave

        Description:      This is our interupt method that determines the wave(s) that are generated through the DAC

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void DAC_Multi_Sine_Wave(void){
    DAC0H = SINE_OFFSET + volume*combined_Sine()/MAX_VOLUME;        /*    Update the voltage in the DAC    */
        /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
}





