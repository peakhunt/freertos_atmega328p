#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "spi_bus_master.h"
#include "tm1638.h"

const uint8_t ASCII[] = 
{		//Code table of symbols

  0x00, 		// (32)  <space>
  0x86, 		// (33)	!
  0x22, 		// (34)	"
  0x7E, 		// (35)	#
  0x6D, 		// (36)	$
  0x00, 		// (37)	%
  0x00, 		// (38)	&
  0x02, 		// (39)	'
  0x30, 		// (40)	(
  0x06, 		// (41)	)
  0x63, 		// (42)	*
  0x00, 		// (43)	+
  0x04, 		// (44)	,
  0x40, 		// (45)	-
  0x80, 		// (46)	.
  0x52, 		// (47)	/
  0x3F, 		// (48)	0
  0x06, 		// (49)	1
  0x5B, 		// (50)	2
  0x4F, 		// (51)	3
  0x66, 		// (52)	4
  0x6D, 		// (53)	5
  0x7D, 		// (54)	6
  0x27, 		// (55)	7
  0x7F, 		// (56)	8
  0x6F, 		// (57)	9
  0x00, 		// (58)	:
  0x00, 		// (59)	;
  0x00, 		// (60)	<
  0x48, 		// (61)	=
  0x00, 		// (62)	>
  0x53, 		// (63)	?
  0x5F, 		// (64)	@
  0x77, 		// (65)	A
  0x7F, 		// (66)	B
  0x39, 		// (67)	C
  0x3F, 		// (68)	D
  0x79, 		// (69)	E
  0x71, 		// (70)	F
  0x3D, 		// (71)	G
  0x76, 		// (72)	H
  0x06, 		// (73)	I
  0x1F, 		// (74)	J
  0x69, 		// (75)	K
  0x38, 		// (76)	L
  0x15, 		// (77)	M
  0x37, 		// (78)	N
  0x3F, 		// (79)	O
  0x73, 		// (80)	P
  0x67, 		// (81)	Q
  0x31, 		// (82)	R
  0x6D, 		// (83)	S
  0x78, 		// (84)	T
  0x3E, 		// (85)	U
  0x2A, 		// (86)	V
  0x1D, 		// (87)	W
  0x76, 		// (88)	X
  0x6E, 		// (89)	Y
  0x5B, 		// (90)	Z
  0x39, 		// (91)	[
  0x64, 		// (92)	"\"
  0x0F, 		// (93)	]
  0x00, 		// (94)	^
  0x08, 		// (95)	_
  0x20, 		// (96)	`
  0x5F, 		// (97)		a
  0x7C, 		// (98)		b
  0x58, 		// (99)		c
  0x5E, 		// (100)	d
  0x7b, 		// (101)	e
  0x31, 		// (102)	f
  0x6F, 		// (103)	g
  0x74, 		// (104)	h
  0x04, 		// (105)	i
  0x0E, 		// (106)	j
  0x75, 		// (107)	k
  0x30, 		// (108)	l
  0x55, 		// (109)	m
  0x54, 		// (110)	n
  0x5C, 		// (111)	o
  0x73, 		// (112)	p
  0x67, 		// (113)	q
  0x50, 		// (114)	r
  0x6D, 		// (115)	s
  0x78, 		// (116)	t
  0x1C, 		// (117)	u
  0x2A, 		// (118)	v
  0x1D, 		// (119)	w
  0x76, 		// (120)	x
  0x6E, 		// (121)	y
  0x47, 		// (122)	z
  0x46, 		// (123)	{
  0x06, 		// (124)	|
  0x70, 		// (125)	}
  0x01 		// (126)	~
};

const uint8_t Num[]= 
{	//Code table of numbers
  0x3F,					//0
  0x06,					//1
  0x5B,					//2
  0x4F,					//3
  0x66,					//4
  0x6D,					//5
  0x7D,					//6
  0x07,					//7
  0x7F,					//8
  0x6F,					//9
  0x77,					//A
  0x7C,					//B
  0x39,					//C
  0x5E,					//D
  0x79,					//E
  0x71					//F
};

const uint8_t ERROR_DATA[] = 
{
  0x79, 		// E
  0x50, 		// r
  0x50, 		// r
  0x5C, 		// o
  0x50, 		// r
  0,
  0,
  0
};

void TM1638_Ports_init()
{
  DDRD  |= _BV(PD2);
  PORTD |= _BV(PD2);

  spi_bus_master_register_slave(0,
      SPIBus_Data_Order_LSB,
      SPIBus_CPOL_High_Idle,
      SPIBus_CPHA_Trailing,
      SPIBus_Speed_DIV_2);
}

static void
tm1683_cs(uint8_t ncs, uint8_t begin_end)
{
  if(begin_end)
  {
    // cs low
    PORTD &= ~_BV(PD2);
  }
  else
  {
    // cs high
    PORTD |= _BV(PD2);
  }
}

void TM1638_SendCommand(unsigned char Command)
{
  spi_bus_master_read_write_sync(0,  &Command, NULL, 1, tm1683_cs);
}

void TM1638_SendData(unsigned int address, unsigned int data)
{
  uint8_t buf[2];

  buf[0] = (unsigned char)(address | 0xc0);
  buf[1] = (unsigned char)data;

  spi_bus_master_read_write_sync(0,  buf, NULL, 2, tm1683_cs);
}

void TM1638_ShowDig(int position, unsigned int Data, int Dot)		//show single digit
{
  TM1638_SendData(position << 1, Num[Data] | (Dot ? 0x80 : 0) );
}

void TM1638_ClearDig(unsigned int position, unsigned int Dot)
{
  TM1638_SendData(position << 1, 0x00 | (Dot ? 0x80 : 0) );
}

void TM1638_ShowError()
{
  unsigned int j;
  for (j=0; j<8; j++) {
    TM1638_SendData(j, ERROR_DATA [j]);
  }
}

void TM1638_ShowDecNumber (unsigned long number, unsigned int dots, unsigned int startingPos)
{
  unsigned int i;

  if (number > 99999999)
  {
    TM1638_ShowError();
  }
  else
  {
    for (i = 0; i < 8 - startingPos; i++)
    {
      TM1638_ShowDig(7 - i, number % 10, (dots & (1 << i)) != 0);
      number /= 10;
    }
  }
}

void TM1638_ShowSignedDecNumber ( signed long number, int dots)
{
  if (number >= 0)
  {
    TM1638_ShowDecNumber (number, dots, 0);
  }
  else
  {
    if (-number > 9999999)
    {
      TM1638_ShowError();
    }
    else
    {
      TM1638_ShowDecNumber(-number, dots, 1);
      TM1638_SendData(0, MINUS);
    }
  }
}

void TM1638_ShowHexNumber (unsigned long number, int dots)
{
  unsigned int i;

  for (i = 0; i < 8; i++)
  {
    TM1638_ShowDig(7 - i, number & 0xF, (dots & (1 << i)) != 0);
    number >>= 4;
  }
}

void TM1638_ShowString (const char * string, unsigned int dots, unsigned int pos)
{
  unsigned int i;

  for (i = 0; i < 8 - pos; i++)
  {
    if (string[i] != '\0')
    {
      TM1638_SendData( (pos + i) << 1, ASCII[ string[i] - 32] | ((dots & (1 << (7 - i ))) ? 0x80 : 0));
    }
    else
    {
      break;
    }
  }
}

void TM1638_ShowLed ( int Number, int Color)
{
  if (Number)
  {
    TM1638_SendData((Number << 1)-1, Color);
  }
}

void TM1638_ShowLeds(int Color)
{
  unsigned int i;

  for (i=1; i<9; i++)
  {
    TM1638_SendData((i << 1)-1, Color);
  }
}

void TM1638_DisplayClean()
{					//Clean RAM of TM1638
  unsigned int i;

  TM1638_SendCommand(DATA_WRITE_INCR_ADDR);		//Set address mode

  for (i = 0; i < 17; i++)
  {
    TM1638_SendData(i, 0x00);
  }
}

void TM1638_SetupDisplay(char active, char intensity)
{
  TM1638_SendCommand (0x80 | (active ? 8 : 0) | intensity);
}

void TM1638_init_Display()
{
  TM1638_DisplayClean();					//Clean display
  TM1638_SendCommand(DISP_OFF);				//Display off
  TM1638_SendCommand(DATA_WRITE_FIX_ADDR);		//Set address mode
  TM1638_SendCommand(ADDRSET);				//Set first address
}

#if 0
unsigned char TM1638_GetKey()
{
  unsigned char   key_in = 0,
                  t;
  int             i;

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

  t = DATA_READ_KEY_SCAN_MODE;
  HAL_SPI_Transmit(&hspi2, &t, 1, 1000);

#if 0
  palSetPadMode(IOPORT2, 3, PAL_MODE_INPUT);
#endif

  for (i = 0; i < 4; i++)
  {
    HAL_SPI_Receive(&hspi2, &t, 1, 1000);
    key_in |= t << i;
  }
#if 0
  palSetPadMode(IOPORT2, 3, PAL_MODE_OUTPUT_PUSHPULL);
#endif


  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);

  return key_in;
}
#endif
