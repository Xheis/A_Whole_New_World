#ifndef LCD
#define LCD
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



/* Function Prototypes */
void initLCD(void);
void clearLCD(void);
void printLCD(unsigned char *);
void writeLCD(unsigned char );
void setCursor(char, char);
void pulse_Enable(unsigned char);

/* Definitions				*/
#define FUNCTION_SET 				0x2
#define	CLR_DISP						0x0
#define CONTROL							0xE
#define ENTRY_MODE_SET			0x6
#define LCD_SEGMENTS				16

sbit DB4 = 				P3 ^ 0;	/* Data bit 4 */
sbit DB5 = 				P3 ^ 1;	/* Data bit 5 */
sbit DB6 = 				P3 ^ 2;	/* Data bit 6 */
sbit DB7 = 				P3 ^ 3;	/* Data bit 7 */
sbit RS = 				P3 ^ 4;	/* Register select */
sbit R_W = 				P3 ^ 5;	/* Read/Write */
sbit ENABLE = 		P3 ^ 6;	/* Enable signal */


#endif