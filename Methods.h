#ifndef Methods
#define Methods

//--------------------------------------------------------------------------------------------------------------------
//                              Function prototypes
//--------------------------------------------------------------------------------------------------------------------

void set_Tone(unsigned short, unsigned char);
void theta_Manager(void);
unsigned short octave_Adjust(unsigned char, unsigned char);
char	combined_Sine(void);
void PB_to_select_Tone();
void UpdateLEDS();
void DAC_Init();
void Timer_Init();
void Voltage_Reference_Init();
void DAC_Sine_Wave();
void DAC_Multi_Sine_Wave();
void Set_Volume(unsigned char);
unsigned char delay_run(unsigned char);
void delay(unsigned short);

#endif