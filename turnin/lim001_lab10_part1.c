/*	Author: lim001
 *  Partner(s) Name: Festo Bwogi
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum threeLEDs{LED1, LED2, LED3} threeState;
enum blinkingLED{BLINK, BLINK2} blinkState;
	
unsigned char three = 0x00;
unsigned char blink = 0x00;


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B 	= 0x0B;	// bit3 = 1: CTC mode (clear timer on compare)
	OCR1A 	= 125;
	TIMSK1 	= 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00;
}	

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 	
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void threeLEDs(){
	switch(threeState){
		case LED1:
			threeState = LED2;
			break;
		case LED2:
			threeState = LED3;
			break;
		case LED3:
			threeState = LED1;
			break;
		default:
			break;
	}		

	switch(threeState){
		case LED1:
			three = 0x01;
			break;
		case LED2:
			three = 0x02;
			break;
		case LED3:
			three = 0x04;
			break;
		default:
			break;
	}			

}

void blinkingLED(){
	switch(blinkState){	
		case BLINK:
			blinkState = BLINK2;
			break;
		case BLINK2:
			blinkState = BLINK;
			break;
		default:
			break;
	}
	
	switch(blinkState){
		
		case BLINK:
			blink = 0x08;
			break;
		case BLINK2:
			blink = 0x00;
			break;
		default:
			break;
	}			

}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);	//change appropriately
    	TimerOn();
    	
    	
		blinkState = BLINK;
		threeState = LED1;
    	
    	while (1)
    	{
	    	
			blinkingLED();
			threeLEDs();
			PORTB = blink | three;
			
			while(!TimerFlag);
			TimerFlag = 0;
    	}
}


























