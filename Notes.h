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



/*    Definitions    */
#define  	WAVE_RESOLUTION    	256   	// Our 256bit sine wave resolution
#define   MAX_VOLUME        	16    	// 16 different volumes
#define   MAX_FADER      			255    	// 256 different fading volumes
#define 	SINE_OFFSET     		128 		// DC offset for sin wave
#define 	DEFAULT_OCTAVE			7
#define 	DEFAULT_VOLUME			15				// Defualt volume. 0-15. 0=> mute
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
								



void DAC_Init();
void Timer_Init();
void Voltage_Reference_Init();
void DAC_Sine_Wave();
void DAC_Multi_Sine_Wave();
void Set_Volume(unsigned char);
void delay_run(unsigned char);
void delay(unsigned short);



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
		SFRPAGE   = TIMER01_PAGE; /* Initialize Timer1 */
    TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x02;					/* Auto Reload Timer		 */
    TL1       = delay_LB[0];
    TH1       = delay_HB[0];
		
		TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x02;
    TL1       = 0x45;
    TH1       = 0x45;
	
	
    SFRPAGE   = TMR2_PAGE;
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


/*		DAC_Sine_Wave			*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Sine_Wave

        Description:      Produces a simple sine wave based on the volume, theta, and d_theta values.

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
/* Run through sine wave */
void DAC_Sine_Wave(void){
		unsigned char i = (unsigned char)((theta[0]&0xFF00)>>8);
		DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*    Update the voltage in the DAC    */
    theta[0] = theta[0] + d_theta[0];    /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
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


void delay(unsigned short delay_len){
	unsigned char num_Run, i;
	num_Run = (delay_len&0xFF00)>>8; /* Number of times to run 8 bit delay */
	for (i=0; i<num_Run; i++){
		delay_run(255);
	}
	delay_run(delay_len&0x00FF); /* Run the remainder of the delay */

}	

void delay_run(unsigned char delay_len){ /* a millisecond delay - caution CPU can do nothing else while running this delay*/
	if(delay_len==0){	/* Just to double check that we havent called a delay which doesnt exsist */
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