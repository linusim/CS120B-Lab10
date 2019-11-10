/* Name & Email: Linus Im
 * Partner(s) Name & Email: Festo Bwogi
 * Lab Section: 022
 * Assignment: Lab 10 Exercise 5
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

#define button0 (~PINA & 0x01)
#define button1 (~PINA & 0x02)

enum States {START, WAIT, INC, DEC, RESET} state;
enum Set_B_States {SetB_Start, Set} B_state;

unsigned char tempB = 0x00;
unsigned long current = 0x00;
unsigned long period = 0x00;

void Button_Hold() {
	if(period == 1000 && current >= 3000)
	period = 400;
	else if(period == 1 && current >= 1000)
	period = 1000;
	if (period == 400 || period == 1000)
	{
		if(state == INC && tempB < 9)
		++tempB;
		else if(state == DEC && tempB > 0)
		--tempB;
	}
}

void Counter() {
	switch(state)	
		case START:
		state = WAIT;
		break;
		case WAIT:
		if(button0 && !button1) {
			state = INC;
			} else if(!button0 && button1) {
			state = DEC;
			} else if(button0 && button1) {
			state = RESET;
			} else {
			state = WAIT;
		}
		break;
		case INC:
		if(button0 && !button1)	{
			state = INC;
			Button_Hold();
			} else if(button0 && button1) {
			state = RESET;
			} else {
			state = WAIT;
			if(tempB < 9) {
				++tempB;
			}
		}
		break;
		case DEC:
		if(!button0 && button1) {
			state = DEC;
			Button_Hold();
			} else if(button0 && button1) {
			state = RESET;
			} else {
			state = WAIT;
			if(tempB > 0) {
				--tempB;
			}
		}
		break;
		case RESET:
		if(!button0 && !button1) {
			state = WAIT;
			} else {
			state = RESET;
		}
		break;
		default:
		break;
	}
	
	switch(state) {
		case START:
		break;
		case WAIT:
		current = 0;
		period = 1;
		break;
		case INC:
		break;
		case DEC:
		break;
		case RESET:
		tempB = 0;
		break;
		default:
		break;
	}
}

void SetB() {
	switch(B_state){
		case SetB_Start:
		B_state = Set;
		break;
		case Set:
		B_state = Set;
		break;
	}
	
	switch(B_state){
		case SetB_Start:
		PORTB = 0x00;
		break;
		case Set:
		PORTB = tempB;
		break;
		default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	unsigned long C_elapsedTime = 1;

	const unsigned long timerPeriod = 1;

	TimerSet(timerPeriod);
	TimerOn();
	
	state = START;
	B_state = SetB_Start;
	period = timerPeriod;

	while(1) {
		if (C_elapsedTime >= period) {
			Counter();
			C_elapsedTime = 0;
		}
		SetB();
		while (!TimerFlag){}
		TimerFlag = 0; 
		C_elapsedTime += timerPeriod;
		current += timerPeriod;
	}
}	
