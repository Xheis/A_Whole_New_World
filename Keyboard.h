#ifndef Keyboard
#define Keyboard

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------			This is our Keyboard.header														---------
---------																														---------
---------			We will store functions related												---------
---------			to taking user input and playing/holding							---------
---------			notes. This will rely on calling upon									---------
---------			the Notes.header for functions/methods								---------
---------			related to generating and playing specific						---------
---------			frequency notes.																			---------
---------																														---------
---------																														---------
---------																														---------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
/* Definitions */
#define		NUM_NOTES		14


/* Function Prototypes */
void set_Tone(unsigned short, unsigned char);
void theta_Manager(void);
unsigned short octave_Adjust(unsigned char, unsigned char);
char	combined_Sine(void);
void PB_to_select_Tone();
void UpdateLEDS();




//void PB_to_select_Tone(void)
//{	
//	//We want to loop this in our main loop, and check our inputs. Ideally, we're going to be reading the P1 input byte for 
//	//changes in value, at which point we can grab the corresponding value. 
//	//The requirements ask us to play the "white keys":
//	//		C 		D 		E		F		G		A		B
//	//		P1.2	P1.3	P1.4	P1.5	P1.6	P1.7	P1.8
//	
//	//When the first button is pressed (P1.1), the sharp keys will be engagued.
//	//		C# 		D#(Eb)	E		F#		G#		A#(Bb)	B
//	//		P1.2	P1.3	P1.4	P1.5	P1.6	P1.7	P1.8
//	// with E & B not having Sharps
//	
//	//As we have 8 buttons, I'd also suggest we make a easy mode to take a selected note, and we can map Octaves 1-8 on P1.
//	//Should be an easy extension *at the end*.
//	
//	//We're done here, proceed to update the display and return to the MainLoop.
//    char tone_select = 0;
//    char i;
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





/* Combined wave ---------------------------------------------------------------------  */
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

	///* Run through sine wave */
	//void DAC_Sine_Wave(void){
	//		unsigned char i = (unsigned char)((theta[0]&0xFF00)>>8);
	//		DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*    Update the voltage in the DAC    */
	//    theta[0] = theta[0] + d_theta[0];    /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
	//}



void set_Tone(unsigned short frequency, unsigned char note_select)
{
    d_theta[note_select] = frequency*4;

}





void UpdateLEDS()
{
	P2 = ~P1; /* Invert Port1 and display it on P2 leds */
}






void DAC_Multi_Sine_Wave(void){
    DAC0H = SINE_OFFSET + volume*combined_Sine()/MAX_VOLUME;        /*    Update the voltage in the DAC    */
        /* Due to sine wave being 8 bit, the char overflow will bring state back to 0 */
}






#endif