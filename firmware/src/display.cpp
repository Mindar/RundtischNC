#include <Arduino.h>
#include <TimerOne.h>
#include <stdio.h>

#include "display.hpp"

#define SEG_B 1 << 0
#define SEG_F 1 << 1
#define SEG_A 1 << 2
#define SEG_G 1 << 3
#define SEG_C 1 << 4
#define SEG_DP 1 << 5
#define SEG_D 1 << 6
#define SEG_E 1 << 7

#define CAT4 5
#define CAT3 6
#define CAT2 7
#define CAT1 8

#define SR_OUTPUT_ENABLE 9
#define SR_LATCH_CLOCK 10 // storage/latch clock
#define SR_RESET 11
#define SR_SHIFT_CLOCK 12
#define SR_SERIAL_DATA 13

char currentDigit = 3;
char displayText[5] = " Loc";


void displayOutput(){
	// Turn off old digit
	digitalWrite(CAT4 + currentDigit, HIGH);

	// Move to next digit
	currentDigit += 1;
	currentDigit = currentDigit >= 4 ? 0 : currentDigit;

	// Prepare latch clock
	digitalWrite(SR_LATCH_CLOCK, LOW);

	shiftOut(SR_SERIAL_DATA, SR_SHIFT_CLOCK, MSBFIRST, getSegments(displayText[((int)currentDigit)]));

	// Latch data and turn on new digit
	digitalWrite(SR_LATCH_CLOCK, HIGH);
	digitalWrite(CAT4 + currentDigit, LOW);
}

void setupDisplay(){
	pinMode(CAT4, OUTPUT);
	pinMode(CAT3, OUTPUT);
	pinMode(CAT2, OUTPUT);
	pinMode(CAT1, OUTPUT);

	pinMode(SR_OUTPUT_ENABLE, OUTPUT);
	pinMode(SR_LATCH_CLOCK, OUTPUT);
	pinMode(SR_RESET, OUTPUT);
	pinMode(SR_SHIFT_CLOCK, OUTPUT);
	pinMode(SR_SERIAL_DATA, OUTPUT);

	digitalWrite(SR_RESET, HIGH);
	digitalWrite(SR_OUTPUT_ENABLE, LOW);

	digitalWrite(SR_LATCH_CLOCK, LOW);
	digitalWrite(SR_SHIFT_CLOCK, LOW);
	digitalWrite(SR_SERIAL_DATA, LOW);


	// Turn off all digits
	digitalWrite(CAT1, HIGH);
	digitalWrite(CAT2, HIGH);
	digitalWrite(CAT3, HIGH);
	digitalWrite(CAT4, HIGH);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(displayOutput);
	Timer1.start();
}

char getSegments(char character) {
	switch (character)
	{
		case '0':
			return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
		case '1':
			return SEG_B | SEG_C;
		case '2':
			return SEG_A | SEG_B | SEG_G | SEG_E | SEG_D;
		case '3':
			return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
		case '4':
			return SEG_F | SEG_B | SEG_C | SEG_G;
		case '5':
			return SEG_A | SEG_F | SEG_G | SEG_C | SEG_D;
		case '6':
			return SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E;
		case '7':
			return SEG_A | SEG_B | SEG_C;
		case '8':
			return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case '9':
			return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
		case 'A':
			return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
		case 'b':
			return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'C':
			return SEG_A | SEG_F | SEG_E | SEG_D;
		case 'd':
			return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
		case 'E':
			return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'F':
			return SEG_A | SEG_E | SEG_F | SEG_G;
		case 'L':
			return SEG_F | SEG_E | SEG_D;
		case 'o':
			return SEG_C | SEG_D | SEG_E | SEG_G;
		case 'c':
			return SEG_E | SEG_D | SEG_G;
		case 'H':
			return SEG_B | SEG_C | SEG_G | SEG_F | SEG_E;
		default:
			return 0x00;
	}
}

void setDisplayText(const char* text) {
	for(int i = 0; i < 4; i++) {
		displayText[i] = text[i];
	}
}

char* setDisplayNumber(int number) {
	if(number < 0 || number > 9999) {
		setDisplayText("FEHL");
	} else {
		sprintf(displayText, "%d", number);
	}

	return displayText;
}