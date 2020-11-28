/*
	Program for the pollution analyser system
	Written by: radvin1378; For Group12, S5LA
	Date: 29/11/2000
*/

#include<reg52.h>

#define LCD_DataBus P2
sbit LCD_EN = P1^0;
sbit LCD_RS = P1^1;


//function prototypes
void delay(int cnt);
void LCDcmd(char cmd);
void LCDdata(char dat);

void main(void)
{
	LCDcmd(0x38);				//Initialize LCD
	LCDcmd(0x0C);				//LCD ON, Cursor OFF
	LCDcmd(0x01);				//Clear display
	LCDcmd(0x80);				//Move curosr to beginning of line 1
	
	delay(1000);
	LCDdata('A');
	LCDdata('A');
	LCDdata('A');

	while(1)
	{
	}
}

//To create Software delay
void delay(int cnt)
{
	int i;
	for(i=0;i<cnt;i++);
}

//To send commands to LCD
void LCDcmd(char cmd)
{
	LCD_DataBus = cmd;	//Sends command to P2
	LCD_RS = 0;					//Makes LCD in Command Mode
	LCD_EN = 1;
	delay(10);
	LCD_EN = 0;					//Send a pulse to Enable pin to latch command
	delay(100);
}

//To send Data to the LCD
void LCDdata(char dat)
{
	LCD_DataBus = dat;	//Sends data to P2
	LCD_RS = 1;					//Makes LCD in Data Mode
	LCD_EN = 1;
	delay(10);
	LCD_EN = 0;					//Send a pulse to Enable pin to latch data
	delay(100);
}
