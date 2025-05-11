/*
 * Final_TETRISCODE.c
 *
 * Created: 5/26/2022 10:32:40 AM
 * Author : DELL
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "max7219.h"
#include "max7219_seg.h"

#define SW4_PRESS !(PINB &(1<<PINB7)) //DOWN button
#define SW3_PRESS !(PINC &(1<<PINC0)) // ROTATE button
#define SW5_PRESS !(PINC &(1<<PINC2)) // RESET button

int all[9][8] = {{0,0,0,0,0,0,0,0},            // 8*8 Matrix
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{1,1,1,1,1,1,1,1}};


int downtime, Cy=0, Cx=0, r = 0, descend=0,  score; //  variables in the code
char val, final_score;

#define USART_BAUDRATE 9600 // Desired Baud Rate
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// USART initializing function
void USART_Init()
{
	// Set Baud Rate
	UBRR0H = BAUD_PRESCALER >> 8;
	UBRR0L = BAUD_PRESCALER;
	
	// Set Frame Format
	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	
	// Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

// Receiving function
uint8_t USART_Receive()
{
	uint8_t DataByte;
	while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
	DataByte = UDR0 ;
	return DataByte;
}

// Transmitting function
void USART_Transmit(uint8_t DataByte)
{
	while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
	UDR0 = DataByte;
}

//text define
void tetris(){
	MAX7219_set_row(1,0b01111100);		//T
	MAX7219_set_column(4,0b01111110);
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111100);
	MAX7219_set_row(3,0b01111100);
	MAX7219_set_row(6,0b01111100);
	MAX7219_set_column(6,0b01111110);//ForE
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111100);		//T
	MAX7219_set_column(4,0b01111110);
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111000);
	MAX7219_set_row(2,0b01000100);
	MAX7219_set_row(3,0b01111000);
	MAX7219_set_column(6,0b01111110);
	MAX7219_set_row(4,0b01100000);
	MAX7219_set_row(5,0b01010000);
	MAX7219_set_row(6,0b01001000);//For R
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111100);
	MAX7219_set_column(4,0b01111110);//For I
	MAX7219_set_row(6,0b01111100);
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b00111100);
	MAX7219_set_row(3,0b00111000);
	MAX7219_set_row(4,0b00000100);
	MAX7219_set_row(5,0b01111000);
	MAX7219_set_column(6,0b00100100);//For s
	_delay_ms(250);
	MAX7219_clear();
	_delay_ms(50);
}


//shapes define
void tetriminos1(){
	if (r == 0 || r== 2 )          //Make a BAR shape Tetriminos
	{
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy,Cx+3,true);
	}
	else if (r == 1 || r == 3)
	{
		MAX7219_set_pixel(Cy,Cx+3,true);
		MAX7219_set_pixel(Cy-1,Cx+3,true);
		MAX7219_set_pixel(Cy-2,Cx+3,true);
		MAX7219_set_pixel(Cy-3,Cx+3,true);
	}

}
void tetriminos2(){
	if (r == 0  )
	{
		MAX7219_set_pixel(Cy,Cx,true);               // L shape
		MAX7219_set_pixel(Cy+1,Cx,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
	}
	else if (r == 1 )
	{
		MAX7219_set_pixel(Cy-1,Cx+2,true);
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
	}
	else if (r == 2 )
	{
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
	}
	else if (r == 3 )
	{
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy-1,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
	}
	
}
void tetriminos3(){
	if (r == 0)
	{
		MAX7219_set_pixel(Cy+1,Cx,true);               //BLOCK 1 shape
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
	}
	else if (r == 1 )
	{
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
	}
	else if (r == 2 )
	{
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx,true);
	}
	else if (r == 3 )
	{
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy-1,Cx,true);
	}
	
}
void tetriminos4(){
	if (r == 0||r == 1||r == 2||r == 3)                 //Square shape
	{
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
	}
	
}
void tetriminos5(){
	if (r == 0||r == 2)
	{
		MAX7219_set_pixel(Cy+1,Cx,true);     //BLOCK2 SHAPE
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
	}
	else if (r == 1||r == 3)
	{
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
	}
	
}
void tetriminos6(){
	if (r == 0||r == 2)      //BLOCK 3 shape
	{
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
	}
	else if (r == 1||r == 3)
	{
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy-1,Cx+2,true);
	}
	
}
void tetriminos7(){
	if (r == 0)
	{
		MAX7219_set_pixel(Cy+1,Cx,true);                    //BLOCK4 SHAPE
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+2,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
	}
	else if (r == 1 )
	{
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
	}
	else if (r == 2 )
	{
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx,true);
		MAX7219_set_pixel(Cy,Cx+2,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
	}
	else if (r == 3 )
	{
		MAX7219_set_pixel(Cy-1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx+1,true);
		MAX7219_set_pixel(Cy+1,Cx+1,true);
		MAX7219_set_pixel(Cy,Cx,true);
	}
	
}

void GAME_OVER(){
	MAX7219_set_row(1,0b00111100); //G
	MAX7219_set_row(2,0b01000000);
	MAX7219_set_row(3,0b01011100);
	MAX7219_set_row(4,0b01000100);
	MAX7219_set_row(5,0b00111100);
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b00110000);//A
	MAX7219_set_row(2,0b01001000);
	MAX7219_set_row(3,0b01111000);
	MAX7219_set_row(4,0b01001000);
	MAX7219_set_row(5,0b01001000);
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_column(2,0b00111110);
	MAX7219_set_column(6,0b00111110);
	MAX7219_set_row(2,0b01101100);
	MAX7219_set_row(3,0b01010100);//M
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111100);
	MAX7219_set_row(3,0b01111100);
	MAX7219_set_row(6,0b01111100);
	MAX7219_set_column(6,0b01111110);//ForE
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_column(2,0b00111100);
	MAX7219_set_column(6,0b00111100);
	MAX7219_set_row(1,0b00111000);
	MAX7219_set_row(6,0b00111000);//O
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(2,0b01000100);
	MAX7219_set_row(3,0b01000100);
	MAX7219_set_row(4,0b00101000);
	MAX7219_set_row(5,0b00010000);//V
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111100);
	MAX7219_set_row(3,0b01111100);
	MAX7219_set_row(6,0b01111100);
	MAX7219_set_column(6,0b01111110);//ForE
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(20);
	
	MAX7219_set_row(1,0b01111000);
	MAX7219_set_row(2,0b01000100);
	MAX7219_set_row(3,0b01111000);
	MAX7219_set_column(6,0b01111110);
	MAX7219_set_row(4,0b01100000);
	MAX7219_set_row(5,0b01010000);
	MAX7219_set_row(6,0b01001000);//For R
	_delay_ms(200);
	MAX7219_clear();
	_delay_ms(500);
	
}

// loop break and shapes store
void loop_break(){
	if (val == '1')
	{
		if (r == 0 || r == 2)
		{
			if (all[Cy+1][Cx] == 1 || all[Cy+1][Cx+1] == 1 || all[Cy+1][Cx+2] == 1 || all[Cy+1][Cx+3] == 1)
			{
				all[Cy][Cx] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy][Cx+3] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 || r == 3)
		{
			if (all[Cy+1][Cx+3] == 1)
			{
				all[Cy][Cx+3] = 1;
				all[Cy-1][Cx+3] = 1;
				all[Cy-2][Cx+3] = 1;
				all[Cy-3][Cx+3] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '2')
	{
		if (r == 0)
		{
			if (all[Cy+2][Cx] == 1 || all[Cy+2][Cx+1] == 1 || all[Cy+2][Cx+2] == 1 )
			{
				all[Cy][Cx] = 1;
				all[Cy+1][Cx] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy+1][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 )
		{
			if (all[Cy+2][Cx+1] == 1 || all[Cy][Cx+2] == 1)
			{
				all[Cy-1][Cx+2] = 1;
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx+1] = 1;
				Cy = 9;
			}
		}
		else if (r == 2 )
		{
			if (all[Cy+2][Cx+2] == 1 || all[Cy+1][Cx+1] == 1 || all[Cy+1][Cx] == 1)
			{
				all[Cy][Cx] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy+1][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 3 )
		{
			if (all[Cy+2][Cx+2] == 1 || all[Cy+2][Cx+1] == 1)
			{
				all[Cy][Cx+2] = 1;
				all[Cy-1][Cx+2] = 1;
				all[Cy+1][Cx+2] = 1;
				all[Cy+1][Cx+1] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '3')
	{
		if (r == 0)
		{
			if (all[Cy+2][Cx] == 1 || all[Cy+2][Cx+1] == 1 || all[Cy+2][Cx+2] == 1 )
			{
				all[Cy+1][Cx] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy+1][Cx+2] = 1;
				all[Cy][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 )
		{
			if (all[Cy+2][Cx+2] == 1 || all[Cy+2][Cx+1] == 1)
			{
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy+1][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 2 )
		{
			if (all[Cy+1][Cx+2] == 1 || all[Cy+1][Cx+1] == 1 || all[Cy+2][Cx] == 1)
			{
				all[Cy][Cx+1] = 1;
				all[Cy][Cx] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy+1][Cx] = 1;
				Cy = 9;
			}
		}
		else if (r == 3 )
		{
			if (all[Cy][Cx] == 1 || all[Cy+2][Cx+1] == 1)
			{
				all[Cy+1][Cx+1] = 1;
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy-1][Cx] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '4')
	{
		if (r == 0 || r == 2 || r == 1 || r == 3)
		{
			if (all[Cy+2][Cx] == 1 || all[Cy+2][Cx+1] == 1 )
			{
				all[Cy][Cx] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx] = 1;
				all[Cy+1][Cx+1] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '5')
	{
		if (r == 0 || r== 2)
		{
			if (all[Cy+2][Cx] == 1 || all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx+2] == 1 )
			{
				all[Cy+1][Cx] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 || r == 3)
		{
			if (all[Cy+2][Cx+2] == 1 || all[Cy+1][Cx+1] == 1)
			{
				all[Cy][Cx+1] = 1;
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy+1][Cx+2] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '6')
	{
		if (r == 0 || r == 2)
		{
			if (all[Cy+2][Cx+2] == 1 || all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx] == 1 )
			{
				all[Cy][Cx] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy+1][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 || r == 3)
		{
			if (all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx+2] == 1)
			{
				all[Cy+1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy-1][Cx+2] = 1;
				Cy = 9;
			}
		}
	}
	else if (val == '7')
	{
		if (r == 0)
		{
			if (all[Cy+2][Cx] == 1 || all[Cy+2][Cx+1] == 1 || all[Cy+2][Cx+2] == 1 )
			{
				all[Cy+1][Cx] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy+1][Cx+2] = 1;
				all[Cy][Cx+1] = 1;
				Cy = 9;
			}
		}
		else if (r == 1 )
		{
			if (all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx+2] == 1)
			{
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy][Cx+2] = 1;
				Cy = 9;
			}
		}
		else if (r == 2 )
		{
			if (all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx+2] == 1 || all[Cy+1][Cx] == 1)
			{
				all[Cy][Cx+1] = 1;
				all[Cy][Cx] = 1;
				all[Cy][Cx+2] = 1;
				all[Cy+1][Cx+1] = 1;
				Cy = 9;
			}
		}
		else if (r == 3 )
		{
			if (all[Cy+2][Cx+1] == 1 || all[Cy+1][Cx] == 1)
			{
				all[Cy-1][Cx+1] = 1;
				all[Cy][Cx+1] = 1;
				all[Cy+1][Cx+1] = 1;
				all[Cy][Cx] = 1;
				Cy = 9;
			}
		}
	}
}

//Fill Display and clear matched row
void Make_board()
{
	int x, y;
	for (int i=0; i<8; i++)
	{
		if (all[i][0] == 1 && all[i][1] == 1 && all[i][2] == 1 && all[i][3] == 1 && all[i][4] == 1 && all[i][5] == 1 && all[i][6] == 1 && all[i][7] == 1)
		{
			score++;
			for (int m= 0; m<8; m++)
			{
				all[i][m] = 0;
			}
			for (y=i-1;y>=0;y--)
			{
				for (x=0;x<8;x++)
				{
					all[y+1][x]= all[y][x];
					
				}
			}
		}
		
	}
	for (y=0;y<9;y++)
	{
		for (x=0;x<8;x++)
		{
			if (all[y][x] == 1)
			{
				MAX7219_set_pixel(y,x,true);
			}
		}
	}
}

//To convert final score to a char
void final_score_count(){
	if(score == 0){
		final_score = '0';
	}else if (score == 1)
	{
		final_score = '1';
	}
	else if (score == 2)
	{
		final_score = '2';
	}
	else if (score == 3)
	{
		final_score= '3';
	}
	else if (score == 4)
	{
		final_score = '4';
	}
	else if (score == 5)
	{
		final_score = '5';
	}
	else if (score == 6)
	{
		final_score = '6';
	}
	else if (score == 7)
	{
		final_score= '7';
	}
	else if (score == 8)
	{
		final_score = '8';
	}
	else if (score == 9)
	{
		final_score = '9';
	}
}

//delay function
void descend_delay_ms(int x)
{
	while (0 < x)
	{
		_delay_ms(1);
		--x;
	}
}




//Random numbers and score display function
void seg(char R){
	if (R== '0')
	{
		MAX7219_set_row_7SEG(0, 0b01111110);
	}
	else if (R == '1')
	{
		MAX7219_set_row_7SEG(0, 0b00110000);
	}
	else if (R == '2')
	{
		MAX7219_set_row_7SEG(0, 0b01101101);
	}
	else if (R == '3')
	{
		MAX7219_set_row_7SEG(0, 0b01111001);
	}
	else if (R == '4')
	{
		MAX7219_set_row_7SEG(0, 0b00110011);
	}
	else if (R == '5')
	{
		MAX7219_set_row_7SEG(0, 0b01011011);
	}
	else if (R == '6')
	{
		MAX7219_set_row_7SEG(0, 0b01011111);
	}
	else if (R == '7')
	{
		MAX7219_set_row_7SEG(0, 0b01110000);
	}
	else if (R == '8')                 //NON-GENERATED RANDOM NUMBERS, only defined
	{
		MAX7219_set_row_7SEG(0, 0b01111111);
	}
	else if (R == '9')
	{
		MAX7219_set_row_7SEG(0, 0b01110111);
	}
}


int main(void)
{
	USART_Init();
	
	EICRA |= (1 << ISC10)|(1 << ISC11);     //  set INT1 to trigger on rising edge
	EIMSK |= (1 << INT1)|(1 << INT0);		// Turns on INT1 & Turns on INT0
	
	
	DDRB &= ~(1<<DDB7);            //pin change Interrupts for down button
	PCMSK0 |= (1 << PCINT7);
	PCICR |= (1 << PCIE0);
	
	DDRC &= ~(1<<DDC0)|~(1<<DDC2);				//pin change Interrupts for Rotate button
	PCMSK1 |= (1 << PCINT8)|(1 << PCINT10);
	PCICR |= (1 << PCIE1);
	
	MAX7219_init();					//Initialize display driver MAX7219 IC.
	MAX7219_set_intensity(20);		//Set brightness of the display.
	
	MAX7219_init_7SEG();
	MAX7219_set_intensity_7SEG(20);
	
	tetris();							//Display TETRIS.
	
	while (1)
	{
		if (all[0][0] == 1 || all[0][1] == 1 || all[0][2] == 1 || all[0][3] == 1 || all[0][4] == 1 || all[0][5] == 1 || all[0][6] == 1 || all[0][7] == 1)
		{
			MAX7219_clear_7SEG();
			final_score_count();
			seg(final_score);
			
			MAX7219_clear();
			GAME_OVER();
		}
		else
		{
			r = 0, downtime = 600, Cx = 2, descend = 9; // blocks will descend at a steady pace with 600ms that will help player to rotate or left, right the blocks
			
			USART_Transmit('T'); //send the capital letter of TETRIS
			val = USART_Receive();
			
			seg(val);
			
			sei();
			
			if (val == '1')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos1();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
			else if (val == '2')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos2();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
					
				}
			}
			else if (val == '3')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos3();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
			else if (val == '4')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos4();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
			else if (val == '5')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos5();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
			else if (val == '6')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos6();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
			else if (val == '7')
			{
				for (Cy=0;Cy<descend;Cy++)
				{
					tetriminos7();
					Make_board();
					descend_delay_ms(downtime);
					MAX7219_clear();
					loop_break();
				}
			}
		}
		
	}
}

ISR(INT0_vect){			//Move left
	if (val == '1') {
		if (Cx < 4)
		{
			if (r == 0 || r == 2)
			{
				if (all[Cy][Cx+4] == 0 )
				{
					Cx++;
				}
			}
			else if (r== 1 || r == 3)
			{
				if (all[Cy][Cx+4] == 0 && all[Cy-1][Cx+4] == 0 && all[Cy-2][Cx+4] == 0 && all[Cy-3][Cx+4] == 0)
				{
					Cx++;
				}
			}
		}
		
		} else  if (val == '4') {
		if (Cx < 6) {
			if (r == 0 || r == 2 || r == 1 || r == 3)
			{
				if (all[Cy][Cx+2] == 0 && all[Cy+1][Cx+2] == 0 )
				{
					Cx++;
				}
			}
		}
	} else if (val == '2' )
	{
		if (Cx < 5) {
			if (r == 0)
			{
				if (all[Cy+1][Cx+3] == 0 && all[Cy][Cx+1] == 0 )
				{
					Cx++;
				}
			}
			else if (r == 1 )
			{
				if (all[Cy-1][Cx+3] == 0 && all[Cy][Cx+2] == 0 && all[Cy+1][Cx+2] == 0)
				{
					Cx++;
				}
			}
			else if (r == 2 )
			{
				if (all[Cy][Cx+3] == 0 && all[Cy+1][Cx+3] == 0)
				{
					Cx++;
				}
			}
			else if (r == 3 )
			{
				if (all[Cy-1][Cx+3] == 0 && all[Cy][Cx+3] == 0 && all[Cy+1][Cx+3] == 0)
				{
					Cx++;
				}
			}
		}
	}
	else if (val == '3' )
	{
		if (Cx < 5) {
			if (r == 0)
			{
				if (all[Cy+1][Cx+3] == 0 && all[Cy][Cx+3] == 0 )
				{
					Cx++;
				}
			}
			else if (r == 1 )
			{
				if (all[Cy+1][Cx+3] == 0 && all[Cy][Cx+2] == 0 && all[Cy-1][Cx+2] == 0)
				{
					Cx++;
				}
			}
			else if (r == 2 )
			{
				if (all[Cy][Cx+3] == 0 && all[Cy+1][Cx+1] == 0)
				{
					Cx++;
				}
			}
			else if (r == 3 )
			{
				if (all[Cy-1][Cx+2] == 0 && all[Cy][Cx+2] == 0 && all[Cy+1][Cx+2] == 0)
				{
					Cx++;
				}
			}
		}
	} else if (val == '5')
	{
		if (Cx < 5)
		{
			if (r == 0 || r== 2)
			{
				if (all[Cy+1][Cx+2] == 0 && all[Cy][Cx+3] == 0 )
				{
					Cx++;
				}
			}
			else if (r == 1 || r == 3)
			{
				if (all[Cy-1][Cx+2] == 0 && all[Cy][Cx+3] == 0 && all[Cy+1][Cx+3] == 0 )
				{
					Cx++;
				}
			}
		}
	}else if (val == '6')
	{
		if (Cx < 5)
		{
			if (r == 0 || r == 2)
			{
				if (all[Cy][Cx+2] == 0 && all[Cy+1][Cx+3] == 0 )
				{
					Cx++;
				}
			}
			else if (r == 1 || r == 3)
			{
				if (all[Cy+1][Cx+2] == 0 && all[Cy][Cx+3] == 0 && all[Cy-1][Cx+3] == 0 )
				{
					Cx++;
				}
			}
		}
	}
	else if (val == '7' )
	{
		if (Cx < 5) {
			if (r == 0)
			{
				if (all[Cy][Cx+2] == 0 && all[Cy+1][Cx+3] == 0 )
				{
					Cx++;
				}
			}
			else if (r == 1 )
			{
				if (all[Cy][Cx+3] == 0 && all[Cy+1][Cx+2] == 0 && all[Cy-1][Cx+2] == 0)
				{
					Cx++;
				}
			}
			else if (r == 2 )
			{
				if (all[Cy][Cx+3] == 0 && all[Cy+1][Cx+2] == 0)
				{
					Cx++;
				}
			}
			else if (r == 3 )
			{
				if (all[Cy-1][Cx+2] == 0 && all[Cy][Cx+2] == 0 && all[Cy+1][Cx+2] == 0)
				{
					Cx++;
				}
			}
		}
	}
}

ISR(INT1_vect){			//Move right
	if (val == '1')
	{
		if (r == 0 || r == 2)
		{
			if (Cx > 0)
			{
				if (all[Cy][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
		
		else if (r == 1 || r == 3)
		{
			if (Cx > -3)
			{
				if (all[Cy][Cx+2] == 0 && all[Cy-1][Cx+2] == 0 && all[Cy-2][Cx+2] == 0 && all[Cy-3][Cx+2] == 0)
				{
					Cx--;
				}
			}
		}
	}
	else  if (val == '4') {
		if (Cx > 0) {
			if (r == 0 || r == 2 || r == 1 || r == 3)
			{
				if (all[Cy][Cx-1] == 0 && all[Cy+1][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
	} else if (val == '2' )
	{
		if (r == 0)
		{
			if (Cx > 0) {
				if (all[Cy+1][Cx-1] == 0 && all[Cy][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
		else if (r == 1 )
		{
			if (Cx > -1) {
				if (all[Cy-1][Cx] == 0 && all[Cy][Cx] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 2 )
		{
			if (Cx > 0) {
				if (all[Cy+1][Cx+1] == 0 && all[Cy][Cx-1] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 3 )
		{
			if (Cx > -1) {
				if (all[Cy-1][Cx+1] == 0 && all[Cy][Cx+1] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
		
	}
	else if (val == '3' )
	{
		
		if (r == 0)
		{
			if (Cx > 0) {
				if (all[Cy][Cx+1] == 0 && all[Cy+1][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
		else if (r == 1 )
		{
			if (Cx > -1) {
				if (all[Cy-1][Cx] == 0 && all[Cy][Cx] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 2 )
		{
			if (Cx > 0) {
				if (all[Cy][Cx-1] == 0 && all[Cy+1][Cx-1] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 3 )
		{
			if (Cx > -1) {
				if (all[Cy-1][Cx-1] == 0 && all[Cy][Cx] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
	} else if (val == '5')
	{
		if (r == 0 || r == 2)
		{
			if (Cx > 0)
			{
				if (all[Cy][Cx] == 0 && all[Cy+1][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
		else if (r == 1 || r == 3)
		{
			if (Cx > -1)
			{
				if (all[Cy-1][Cx] == 0 && all[Cy][Cx] == 0 && all[Cy+1][Cx+1] == 0 )
				{
					Cx--;
				}
			}
		}
		
	}else if (val == '6')
	{
		if (r == 0 || r == 2)
		{
			if (Cx > 0)
			{
				if (all[Cy][Cx-1] == 0 && all[Cy+1][Cx] == 0 )
				{
					Cx--;
				}
			}
		}
		else if (r == 1 || r == 3)
		{
			if (Cx > -1)
			{
				if (all[Cy+1][Cx] == 0 && all[Cy][Cx] == 0 && all[Cy-1][Cx+1] == 0 )
				{
					Cx--;
				}
			}
		}
		
	}
	else if (val == '7' )
	{
		if (r == 0)
		{
			if (Cx > 0) {
				if (all[Cy][Cx] == 0 && all[Cy+1][Cx-1] == 0 )
				{
					Cx--;
				}
			}
		}
		else if (r == 1 )
		{
			if (Cx > -1) {
				if (all[Cy][Cx] == 0 && all[Cy+1][Cx] == 0 && all[Cy-1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 2 )
		{
			if (Cx > 0) {
				if (all[Cy][Cx-1] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
		else if (r == 3 )
		{
			if (Cx > 0) {
				if (all[Cy-1][Cx] == 0 && all[Cy][Cx-1] == 0 && all[Cy+1][Cx] == 0)
				{
					Cx--;
				}
			}
		}
	}
}

//SW3 button(Rotate) Pin Change Interrupts ISR
ISR(PCINT1_vect){
	if( SW3_PRESS )
	{
		if (r == 0)
		{
			r = 1;
		}
		else if (r == 1)
		{
			r = 2;
		}
		else if (r == 2)
		{
			r = 3;
		}
		else if (r == 3)
		{
			r = 0;
		}
	}
	if (SW5_PRESS)
	{
		score = 0;
		if (all[0][0] == 1 || all[0][1] == 1 || all[0][2] == 1 || all[0][3] == 1 || all[0][4] == 1 || all[0][5] == 1 || all[0][6] == 1 || all[0][7] == 1)
		{
			for (int y=0;y<8;y++)
			{
				for (int x=0;x<8;x++)
				{
					if (all[y][x] == 1)
					{
						all[y][x] = 0;
					}
				}
			}
		}
	}
}

//SW4 Button(to down) Pin Change Interrupts ISR
ISR(PCINT0_vect){
	if( SW4_PRESS )
	{
		downtime = 20;	// when button pressed the blocks descend fast
	}
	else
	{
		downtime= 600;  // when button not pressed or released it descends normally
	}
}
