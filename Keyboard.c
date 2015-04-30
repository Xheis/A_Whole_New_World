#include "Keyboard.h"


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
	alteredPort = P1&0xFE; 	/* Initially disregard pushbutton 1 */
	unsigned char button_i;
	unsigned char i,j;
	unsigned short tone;
	unsigned char buttons_active = 0;
	UpdateLEDS(); /* Display the notes pushed on the keyboard */
	
	for(i = 1; i<8; i++){ 	/* Check buttons 1 to 7 inclusive */
		button_i = ((alteredPort>>i)+1)%2; /* Move value to the right then increment by one then take the modulo to check whether button is active */
		/* button_i ACTIVE HIGH */
		buttons_active += button_i; /* Count the number of active notes */
		if(button_i){/* a key has been pressed */
			TR2 = 1;		/* Run timer2 if a button is set */
			j = i-1; /* Normalize for pointing to an array */
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

unsigned char	combined_Sine(void){
	unsigned char i,j; 
	unsigned short sine_combined = 0; /* The combined value of the notes */
	for(i = 0; i< NUM_NOTES; i++){
		j = (unsigned char)((theta[i]&0xFF00)>>8);
		sine_combined += sin[j];					/* Add fader control here */
		theta[i] = theta[i] + d_theta[i]; /* Increment theta */
	}
	return((unsigned char)(sine_combined/num_active_keys)); /* Return the new DAC Value */
}

void set_Tone(unsigned short frequency, unsigned char note_select)
{
    d_theta[note_select] = frequency;
}


void UpdateLEDS()
{
	P2 = ~P1; /* Invert Port1 and display it on P2 leds */
}

