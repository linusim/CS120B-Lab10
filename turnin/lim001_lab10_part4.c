/*	Author: lim001
 *  Partner(s) Name: Festo Bwogi
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char combined = 0;

enum BL_states{BL_start, BL_off, BL_on} BL_state;
void Tick_BL();
static unsigned char blink = 0;

enum Three_states{Three_start, Three_1, Three_2, Three_3} Three_state;
void Tick_Three();
static unsigned char three = 0;

enum Sound_states{sound_start, sound_off, sound_add, sound_sub, sound_play} Sound_state;
void Tick_Sound();
unsigned char buttonAdd = 0x00;
unsigned char buttonSub = 0x00;
unsigned char buttonPlay = 0x00;
static unsigned long soundPlay = 0;
unsigned char intCnt = 0;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

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
	TimerFlag = 1;	
}

ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; }
		else { TCCR0B |= 0x03; }

		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		else if (frequency > 31250) { OCR0A = 0x0000; }
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT0 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; //input
	DDRB = 0xFF; PORTB = 0x00; //output
	
	unsigned long timerBL = 1000;
	unsigned long timerThree = 300;
	const unsigned long timeSet = 100;
	
	TimerSet(timeSet);
	TimerOn();
	PWM_on();
	
	BL_state = BL_start;
	Three_state = Three_start;
	Sound_state = sound_start;
	while (1)
	{
		if (timerBL >= 1000 ) {
			Tick_BL();
			timerBL = 0;
		}
		if (timerThree >= 300) {
			Tick_Three();
			timerThree = 0;
		}
		Tick_Sound();
		combined = blink + three;
		PORTB = combined;
		while(!TimerFlag){};
		TimerFlag = 0;
		timerBL += timeSet;
		timerThree += timeSet;
	}
}

void Tick_BL() {
	switch(BL_state) {
		case BL_start:
		BL_state = BL_off;
		break;
		
		case BL_off:
		BL_state = BL_on;
		break;
		
		case BL_on:
		BL_state = BL_off;
		break;
		
		default:
		BL_state = BL_start;
		break;
	}
	switch(BL_state) {
		case BL_start:
		blink = 0x00;
		break;
		
		case BL_off:
		blink = 0x00;
		break;
		
		case BL_on:
		blink = 0x10;
		break;
		
		default:
		blink = 0x00;
		break;
	}
}

void Tick_Three() {
	switch(Three_state) {
		case Three_start:
		Three_state = Three_1;
		break;
		
		case Three_1:
		Three_state = Three_2;
		break;
		
		case Three_2:
		Three_state = Three_3;
		break;
		
		case Three_3:
		Three_state = Three_1;
		break;
		
		default:
		Three_state = Three_1;
		break;
	}
	switch(Three_state) {
		case Three_start:
		three = 0x00;
		break;
		
		case Three_1:
		three = 0x01;
		break;
		
		case Three_2:
		three = 0x02;
		break;
		
		case Three_3:
		three = 0x04;
		break;
		
		default:
		three = 0x00;
		break;
	}
}

void Tick_Sound (){
	buttonPlay = ~PINA & 0x01;
	buttonAdd = ~PINA & 0x02;
	buttonSub = ~PINA & 0x04;
	switch(Sound_state) {
		case sound_start:
		if(buttonAdd) {
			Sound_state = sound_add;
		}
		else if(buttonSub) {
			Sound_state = sound_sub;
		}
		else if (buttonPlay) {
			Sound_state = sound_play;
		}
		else {
			Sound_state = sound_off;
		}
		break;
		case sound_off:
		Sound_state = sound_start;
		break;
		
		case sound_add:
		Sound_state = sound_start;
		break;
		
		case sound_sub:
		Sound_state = sound_start;
		break;
		
		case sound_play:
		Sound_state = sound_start;
		break;
		
		default:
		Sound_state = sound_start;
		break;
	}
	switch(Sound_state) {
		case sound_start:
		break;
		
		case sound_off:
		soundPlay = 0;
		set_PWM(soundPlay);
		break;
		
		case sound_add:
		if (intCnt < 9) {
			++intCnt;
		}
		break;
		
		case sound_sub:
		if (intCnt > 0) {
			--intCnt;
		}
		break;
		
		case sound_play:
		if (intCnt == 1) {
			set_PWM(261.63);
		}
		else if (intCnt == 2) {
			set_PWM(293.66);
		}
		else if (intCnt == 3) {
			set_PWM(329.63);
		}
		else if (intCnt == 4) {
			set_PWM(349.23);
		}
		else if (intCnt == 5) {
			set_PWM(392.00);
		}
		else if (intCnt == 6) {
			set_PWM(440.00);
		}
		else if (intCnt == 7) {
			set_PWM(493.88);
		}
		else if (intCnt == 8) {
			set_PWM(523.25);
		}
		break;
		
		default:
		soundPlay = 0;
		set_PWM(soundPlay);
		break;
	}
}
