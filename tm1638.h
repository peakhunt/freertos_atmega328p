#ifndef __TM1638_H
#define __TM1638_H

#define TM1638_LED_RED 		0x01		// RED Led
#define TM1638_LED_GRE 		0x02		// Green Led
#define TM1638_LED_YEL 		0x03		// Both Led's ON
#define TM1638_LED_NON 		0x00		// Both Led's OFF

#define TM1638_KEY1       0x01    // Key1
#define TM1638_KEY2       0x02    // Key2
#define TM1638_KEY3       0x04    // Key3
#define TM1638_KEY4       0x08    // Key4
#define TM1638_KEY5       0x10    // Key5
#define TM1638_KEY6       0x20    // Key6
#define TM1638_KEY7       0x40    // Key7
#define TM1638_KEY8       0x80    // Key8

#define  DEGREE         0x63	// "Degree" char
#define  MINUS          0x40    // "Minus" char

/* TM1638 Commands Define  */
#define DATA_WRITE_INCR_ADDR 	  0x40		// Command to switch TM1638 for automatic increment address mode
#define DATA_WRITE_FIX_ADDR 	  0x44		// Command to switch TM1638 for fix address mode
#define DATA_READ_KEY_SCAN_MODE 0x42		// Command for read key code from TM1638
#define ADDRSET 		            0xC0		// Command to set address 0x00
#define DISP_ON 		            0x8F		// Command to Display ON and set (max) brightness
#define DISP_OFF 		            0x80		// Command to Display OFF

extern void TM1638_Ports_init(void);
extern void TM1638_SendCommand(unsigned char Command);
extern void TM1638_SendData(unsigned int address, unsigned int data);
extern void TM1638_ShowDig(int position, unsigned int Data, int Dot);
extern void TM1638_ClearDig (unsigned int position, unsigned int Dot);
extern void TM1638_ShowError(void);
extern void TM1638_ShowDecNumber (unsigned long number, unsigned int dots, unsigned int startingPos);
extern void TM1638_ShowSignedDecNumber ( signed long number, int dots);
extern void TM1638_ShowHexNumber (unsigned long number, int dots);
extern void TM1638_ShowString (const char * string, unsigned int dots, unsigned int pos);
extern void TM1638_ShowLed ( int Number, int Color);
extern void TM1638_ShowLeds(int Color);
extern void TM1638_DisplayClean(void);
extern void TM1638_SetupDisplay(char active, char intensity);
extern void TM1638_init_Display(void);
extern unsigned char TM1638_GetKey(void);

#endif //!__TM1638_H
