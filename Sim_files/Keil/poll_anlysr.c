/*
	Program for pollution analyser system using 8051 uP and PPD42NS dust sensor
	Author: radvin1378
	Date: 29/11/2000
*/

#include <REGX51.H>

#define sampletime 10
#define LCD_DataBus P2
sbit LCD_EN = P1^0;
sbit LCD_RS = P1^1;
sbit ppd = P1^2;

//function prototypes
void delay(int cnt);
void LCD_cmd(char cmd);
void LCD_data(char dat);
void LCD_init(void);
void LCDWrite(void);
unsigned lpotime(unsigned a, unsigned b, unsigned cntr);

//global variables
float ratio, LPO=0;
char line1[11] = "Dust Level:";
char plevel[3][6] = {"LOW","MEDIUM","HIGH"};

//main function
void main(void)
{
	int a,b,i,k=0;
	unsigned cntr=0;
	LCD_init();			//Initialising LCD	
	
	//Timer1 --> 10 sec interval; Timer0 --> PWM
	TMOD = 0x11;			//Configured Timer0 & Timer1 to timer type in Mode1; TMOD=[00010001]
	TH1 = 0x5D;			//Initialised Timer1 High part to 5D	(0x5DB1)-->(23,985)
	TL1 = 0xB1;			//Initialised Timer1 Low part to B1
	TH0 = 0x00;			//Initialised Timer0 High part to E0
	TL0 = 0x00;			//Initialised Timer0 Low part to 00
	
	while(1)
	{
		TR1 = 1;			//Starts Timer1 --> 10 second timer
		for(i=0;i<141;i++)
		{
			while(TF1 == 0)
			{
				if(ppd == 1 && k == 0)
				{
					TR0=0;			//Stops Timer0 --> PWM
					a = TH0;
					b = TL0;
					TH0 = 0x00;			//Resets TH0
					TL0 = 0x00;			//Resets Tl0
					LPO += lpotime(a,b,cntr);			//Updates LPO
					k = 1;
				}
				else if (ppd == 0 && k == 1)
				{
					TR0=1;			//Starts Timer0 --> PWM
					k=0;
					while(ppd == 0)
					{
						if(TF0 != 0)			//if Timer0 overflows
						{
							cntr++;
							TF0 = 0;			//Resets Overflow flag of Timer0
						}
					}
				}
			}
			TF1=0;			//Resets Overflow flag of Timer1 --> 10 sec
		}
		TR1=0;			//Stops Timer1 --> End of 10 seconds
		ratio = LPO/100;			//ratio = (LPO(ms)/10000(10 sec in ms))*100
		LCDWrite();			//Displays status after 10 seconds
		LPO = 0;			//Resets LPO for new interval
		cntr = 0;			//Resets cntr variable
	}
}

//To create Software delay
void delay(int cnt)
{
	int i;
	for(i=0;i<cnt;i++);
}

//To send commands to LCD
void LCD_cmd(char cmd)
{
	LCD_DataBus = cmd;			//Sends command to P2
	LCD_RS = 0;			//Makes LCD in Command Mode
	LCD_EN = 1;
	delay(10);
	LCD_EN = 0;			//Send a pulse to Enable pin to latch command
	delay(100);
}

//To initialise the LCD
void LCD_init(void)
{
	LCD_cmd(0x38);			//Initialize LCD
	LCD_cmd(0x0C);			//LCD ON, Cursor OFF
	LCD_cmd(0x01);			//Clear display
	LCD_cmd(0x80);			//Move curosr to beginning of line 1
}	

//To send Data to the LCD
void LCD_data(char dat)
{
	LCD_DataBus = dat;			//Sends data to P2
	LCD_RS = 1;			//Makes LCD in Data Mode
	LCD_EN = 1;
	delay(10);
	LCD_EN = 0;			//Send a pulse to Enable pin to latch data
	delay(100);
}

//converts timer0 data to time in ms (PWM)
unsigned lpotime(unsigned a, unsigned b, unsigned cntr)
{
	unsigned c,d,result;
	c = a%16;			//LSB of TH0
	a = a/16;			//MSB of TH0
	d = b%16;			//LSB of TL0
	b = b/16;			//MSB of TL0
	result = (a*(16*16*16)+c*(16*16)+b*(16)+d)*0.001085 + cntr*71.1;			//Converts no.of clocks to millisecs
	return result;
}

//Write the result to LCD
void LCDWrite(void)
{
	int i,j;
	
	if (ratio < 5.0405)
	{
		LCD_cmd(0x01);			//Clear display
		LCD_cmd(0x83);			//Move cursor to first row and 3th column
		for (i=0;i<11;i++)			//Prints "Pollution Level:" on line 1
			LCD_data(line1[i]);
		
		LCD_cmd(0xC7);			//Move cursor to second row and 6th column
		for(j=0;j<6;j++)
			LCD_data(plevel[0][j]);			//Prints "LOW"
	}

	else if(ratio > 5.0405 && ratio < 9.2170)
	{
		LCD_cmd(0x01);			//Clear display
		LCD_cmd(0x83);			//Move cursor to first row and 3th column
		for (i=0;i<11;i++)			//Prints "Pollution Level:" on line 1
			LCD_data(line1[i]);
		
		LCD_cmd(0xC5);			//Move cursor to second row and 5th column
		for(j=0;j<6;j++)
			LCD_data(plevel[1][j]);			//Prints "MEDIUM"
	}

	else	// ratio > 9.2170
	{
		LCD_cmd(0x01);			//Clear display
		LCD_cmd(0x83);			//Move cursor to first row and 4th column
		for (i=0;i<11;i++)			//Prints "Pollution Level:" on line 1
			LCD_data(line1[i]);
		
		LCD_cmd(0xC6);			//Move cursor to second row and 6th column
		for(j=0;j<6;j++)
			LCD_data(plevel[2][j]);			//Prints "HIGH"	
	}
}