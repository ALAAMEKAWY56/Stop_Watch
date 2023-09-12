/**********************************************************************************************************
 * [FILE NAME]		:			Mini_Project2.c													          *
 * [DESCRIPTION]	:			Stop Watch Project											              *
 * [DATE CREATED]	: 			Sep 11, 2023															  *
 * [AURTHOR]		: 			Alaa Mekawi															      *
 *********************************************************************************************************/

/************************************Includes libraries*********************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* global variables for time */
unsigned char Sec_1= 0;
unsigned char Sec_2 = 0;
unsigned char Min_1 = 0;
unsigned char Min_2= 0;
unsigned char Hour_1 = 0;
unsigned char Hour_2 = 0;

void display_7seg(void){

	/* enable PA0 7 seg to display  Sec_1 */
	PORTA = (PORTA & 0xC0) | (1<<PA0);
	/*then display Sec_1 on decoder */
	PORTC = (PORTC & 0xF0) | (Sec_1 & 0x0F);

	/* delay 3 mili */
	_delay_ms(3);

	/* enable PA1 7 seg to display  Sec_2 */
	PORTA = (PORTA & 0xC0) | (1<<PA1);
	/*then display Sec_2 on decoder */
	PORTC = (PORTC & 0xF0) | (Sec_2 & 0x0F);

	_delay_ms(3);

	/* enable PA2 7 seg to display  Min_1 */
	PORTA = (PORTA & 0xC0) | (1<<PA2);
	/*then display Min_1 on decoder */
	PORTC = (PORTC & 0xF0) | (Min_1 & 0x0F);

	_delay_ms(3);

	/* enable PA3 7 seg to display  Min_2 */
	PORTA = (PORTA & 0xC0) | (1<<PA3);
	/*then display Min_2 on decoder */
	PORTC = (PORTC & 0xF0) | (Min_2 & 0x0F);

	_delay_ms(3);

	/* enable PA4 7 seg to display  Hour_1 */
	PORTA = (PORTA & 0xC0) | (1<<PA4);
	/*then display Min_2 on decoder */
	PORTC = (PORTC & 0xF0) | (Hour_1 & 0x0F);

	_delay_ms(3);

	/* enable PA5 7 seg to display  Hour_2 */
	PORTA = (PORTA & 0xC0) | (1<<PA5);
	/*then display Hour_2 on decoder */
	PORTC = (PORTC & 0xF0) | (Hour_2 & 0x0F);

	_delay_ms(3);
}

/* Timer1 Compare mode enable and configuration function */
void Timer1_COMP(void){
	/* Set timer1 initial count to zero */
	TCNT1 = 0;

	/* Set the Compare value to 978 -> to count one sec with N= 1024 & Fcpu = 1MHZ */
	OCR1A = 978;
	TCCR1A = (1<<FOC1A);

	/* Mode 4
	 * clkI/O/1024 (From prescaler) */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);

	/* Enable Timer1 Compare A Interrupt */
	TIMSK |= (1<<OCIE1A);

}

/* Interrupt Service Routine for timer1 compare mode */
ISR(TIMER1_COMPA_vect){
	/* with every second increment Sec_1 each interrupt */
	Sec_1++;
	if(Sec_1 == 10)
	{
		Sec_1 = 0;
		Sec_2++;
	}
	/* if reached 60 seconds increment one minute */
	if(Sec_2 == 6 && Sec_1 == 0)
	{
		Sec_2 = 0;
		Min_1++;
	}
	if(Min_1 == 10)
	{
		Min_1 = 0;
		Min_2++;
	}
	/* if reached 60 Minutes increment one Hour */
	if(Min_2 == 6 && Min_1 == 0)
	{
		Min_2 = 0;
		Hour_1++;
	}
	if(Hour_1 == 10)
	{
		Hour_1 = 0;
		Hour_2++;
	}
}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	// Configure INT0/PD2 as input pin
	DDRD  &= ~(1<<PD2);

	//Enable internal pull up
	PORTD |= (1<<PD2);

	// Trigger INT0 with the Falling edge
	MCUCR |= (1<<ISC01);

	// Enable external interrupt pin INT0
	GICR  |= (1<<INT0);
}

/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	/* Reset all variables to zeroes*/
	Sec_1=0;
	Sec_2=0;
	Min_1=0;
	Min_2=0;
	Hour_1=0;
	Hour_2=0;

}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	// Configure INT1/PD3 as input pin
	DDRD  &= ~(1<<PD3);

	// Trigger INT1 with the Rising edge
	MCUCR |= (1<<ISC11) | (1<<ISC10);

	// Enable external interrupt pin INT1
	GICR  |= (1<<INT1);
}

/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	/*stop timer -> disable it's clk */
	TCCR1B &= ~(1<<CS10) & ~(1<<CS11) & ~(1<<CS12);

}

/* External INT2 enable and configuration function */
void INT2_Init(void)
{
	// Configure INT2/PB2 as input pin
	DDRB  &= ~(1<<PB2);

	//Enable internal pull up
	PORTB |= (1<<PB2);

	// Trigger INT2 with the Falling edge
	MCUCSR &= ~(1<<ISC2);

	// Enable external interrupt pin INT2
	GICR  |= (1<<INT2);
}

/* External INT2 Interrupt Service Routine */
ISR(INT2_vect)
{
	/*Enable timer again -> enable it's clk */
	TCCR1B |= (1<<CS10) | (1<<CS12);
}

int main(void){
	/* Enable global interrupts in MC */
	SREG |= (1<<7);

	/* define first Four pins in PORTC PC0,PC1,PC2,PC3 as output */
	DDRC |= 0x0F;

	/* give PC0,PC1,PC2,PC3 initial value as OFF */
	PORTC &= 0xF0;

	/* define first six pins in PORTA as output */
	DDRA |= 0x3F;

	/* give first six pins in PORTA initial value as 0 "disabled" */
	PORTA &= 0xC0;

	/*Enable timer1 */
	Timer1_COMP();

	/*Enable INT0 */
	INT0_Init();

	/*Enable INT1 */
	INT1_Init();

	/*Enable INT2 */
	INT2_Init();

	while(1){

		//display_time
		display_7seg();

	}


}

