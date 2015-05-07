// LCD.c - This .c file contains the declarations 
// for the LCD functions 
#include "c8051F120.h"
#include "Assign2.h"
#include "Methods.h"
#include "LCD.h"

//unsigned char print_mem[LCD_SEGMENTS + 1]; /* Number of characters plus the NULL character */

void initLCD(){
	/* Power On */
	delay(20);
	
	P3 = FUNCTION_SET;
	pulse_Enable(5);
	P3 = FUNCTION_SET;
	pulse_Enable(5);
	
	/* Display ON/OFF */
	P3 = CLR_DISP;
	pulse_Enable(5);
	P3 = CONTROL;
	pulse_Enable(5);
	
	/* Entry Mode Set */
	P3 = CLR_DISP;
	pulse_Enable(5);
	P3 = ENTRY_MODE_SET;
	pulse_Enable(5);
	
	/* Now we can write data! YAH */
	
}

void clearLCD(){
}

void printLCD(unsigned char *word){

	while(*word){ /* print character by character till we reach the NULL character */
		writeLCD(*word++); /* increment pointer to the next character in the word */
	}
}

void writeLCD(unsigned char character){ /* Write a character to the LCD screen */
	
	P3 = (character>>4)&(0x0F); /* Upper nibble of character */
	pulse_Enable(1);
	P3 = (character)&(0x0F); 		/* Lower nibble of character */
	pulse_Enable(1);
}

void setCursor(char x, char y){
	////// Something
}

void pulse_Enable(unsigned char i){
	ENABLE = 1;
	delay(i);
	ENABLE = 0;
	delay(i);
}
