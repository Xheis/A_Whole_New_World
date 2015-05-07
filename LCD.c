// LCD.c - This .c file contains the declarations 
// for the LCD functions 
#include "c8051F120.h"
#include "Assign2.h"
#include "Methods.h"
#include "LCD.h"

//unsigned char print_mem[LCD_SEGMENTS + 1]; /* Number of characters plus the NULL character */
unsigned char keepButtonState;

void initLCD(){
	/* Power On */
	delay(30);

	P3 = FUNCTION_SET;
	pulse_Enable(5);

	cmdLCD(TWO_LINE_CONTROL);
	
	/* Display ON/OFF */
	cmdLCD(CONTROL);
	
	/* Entry Mode Set */
	cmdLCD(ENTRY_MODE_SET);
	
	cmdLCD(CLR_DISP);
	printLCD("    ELEC2700    ");
	delay(500);
	cmdLCD(CLR_DISP);

	MPB = 1;
	
	/* Now we can write data! YAH */
	
}

void clearLCD(){
unsigned char clearChar = 0x01;
unsigned char returnChar = 0x02;
writeLCD(clearChar);
writeLCD(returnChar);
}

void printLCD(unsigned char *word){
	int fill = 0;
	int i = 0;
	while(*word){ /* print character by character till we reach the NULL character */
		fill++;
		writeLCD(*word++); /* increment pointer to the next character in the word */
		
	}
	for(fill; fill<16; fill++)
	{
	writeLCD(' ');
	}

}

void writeLCD(unsigned char character){ /* Write a character to the LCD screen */
	/* Send first Nibble */
	P3 = 0;
	RS = 1;
	P3 = P3|(character>>4)&(0x0F); /* Upper nibble of character */
	pulse_Enable(1);
	
	/* Send last Nibble */
	P3 = 0;
	RS = 1;
	P3 = P3|(character)&(0x0F); 		/* Lower nibble of character */
	pulse_Enable(1);
	MPB = 1;
	RS = 0;
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

void cmdLCD(unsigned char cmd){
		/* Send first Nibble */
	P3 = 0;
	P3 = P3|(cmd>>4)&(0x0F); /* Upper nibble of character */
	pulse_Enable(3);
	
	/* Send last Nibble */
	P3 = 0;
	P3 = P3|(cmd)&(0x0F); 		/* Lower nibble of character */
	pulse_Enable(3);
	MPB = 1;
}


