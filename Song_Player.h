#ifndef MIDI_Player
#define MIDI_Player
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------			This is our MIDI_Player.header					---------
---------															---------
---------			We will store functions related					---------
---------			to interpreting MIDI files &					---------
---------			playing them here.								---------
---------															---------
---------			These functions will all route through			---------
---------			a single function called "PlayMIDI(SongName)"	---------
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

//Prototypes:
void DAC_Sine_Wave();

/* Tones and their frequencies 		C		C#		D		Eb		E		F		F#		G		G#		A		Bb		B	*/
unsigned short	 code	tone[]	=	{262,	277,	294,	311,	330,	349,	370,	392,	415,	440,	466,	494};

const char    code    sin[] = { 
                                    /* DAC voltages for 8-bit, 16 volume sine wave */
                                    /* ------------------------------------------------------------------------ */
                                    
                                    #include "sine_8_bits.csv"    /* 256 piece sine wave */
                                };


/*		Voltage_Reference_Init	*/
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Voltage_Reference_Init

        Description:      Initialise voltage references (Needed for DAC)

--------------------------------------------------------------------------------------------------------------------*/

void DAC_Sine_Wave(void){
    unsigned char i = (unsigned char)((theta&0xFF00)>>8);	/* 		Our theta value is our phase, and is of a much larger 
															resolution then our DAC0H will accept. So let's get the 8 MSBs*/
    DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*     Update the voltage in the DAC    */
    theta = theta + Dtheta;   								 /* 	Here we are stepping through theta. 
															Due to sine wave being 8 bit, the char will overflow
															and will consequently bring our state back to 0 */ne wave being 8 bit, the char overflow will bring state back to 0 */
}


//Let's make a C Major Chord
void PlayCMajor	(void)
{
    unsigned char i = (unsigned char)((theta&0xFF00)>>8);	/* 		Our theta value is our phase, and is of a much larger 
															resolution then our DAC0H will accept. So let's get the 8 MSBs*/
    DAC0H = SINE_OFFSET + volume*(sin[i])/MAX_VOLUME;        /*     Update the voltage in the DAC    */
    theta = theta + tone[0]+tone[4]+tone[7];   								 /* 	Here we are stepping through theta. 
															Due to sine wave being 8 bit, the char will overflow
															and will consequently bring our state back to 0 */
}

/* Tones and their frequencies 		C		C#		D		Eb		E		F		F#		G		G#		A		Bb		B	*/
//unsigned shcode	tone[]	=		{262,	277,	294,	311,	330,	349,	370,	392,	415,	440,	466,	494};

#endif