/*	Author: lim001
 *  Partner(s) Name: Festo Bwogi
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct task {
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int(*TickFct)(int);
} task;

task tasks[2];	
const unsigned short numTasks = 2;
const unsigned long tasksPeriodGCD = 100;
const unsigned long BL_Period = 1000;
const unsigned long TL_Period = 300;

enum BL_States {BL_START, OFF, ON} BL_STATE;
enum TL_States {TL_START, LED1, LED2, LED3} TL_STATE;
	
unsigned char BL_B  = 0x00;
unsigned char TL_B = 0x00;

volatile unsigned char TimierFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

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
	unsigned char i;
	
	for(i = 0; i < numTasks; ++i) {
		if(tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}

nt BlinkLED(int BL_STATE) {
	switch(BL_STATE) { // Transition actions
		case BL_START:
		case BL_START:
		BL_STATE = ON;
		break;
		case OFF:
		BL_STATE = ON;
		break;
		case ON:
		BL_STATE = OFF;
		break;
	}

	switch(BL_STATE) { // State actions
		case OFF:
		BL_B = 0x00;
		break;
		case ON:
		BL_B = 0x08;
		break;
		default:
		break;
	}
	
	return BL_STATE;
}

int ThreeLED(int TL_STATE) {
	switch(TL_STATE) { // Transition actions
		case TL_START:
		TL_STATE = LED1;
		break;
		case LED1:
		TL_STATE = LED2;
		break;
		case LED2:
		TL_STATE = LED3;
		break;
		case LED3:
		TL_STATE = LED1;
		break;
	}

		TL_B = 0x01;
		break;
		case LED2:
		TL_B = 0x02;
		break;
		case LED3:
		TL_B = 0x04;
		break;
		default:
		break;
	}
	
	return TL_STATE;
}

int main(void) {
	unsigned char i = 0;
	
	tasks[i].state = BL_START;
	tasks[i].period = BL_Period;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &BlinkLED;
	i++;
	tasks[i].state = TL_START;
	tasks[i].period = TL_Period;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &ThreeLED;
	
	TimerSet(100);
	TimerOn();
	
	while (1)
	{
		PORTB = BL_B | TL_B;
	}
	
	return 0;
}

























