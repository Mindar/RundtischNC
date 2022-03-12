#include <Arduino.h>
#include <AccelStepper.h>

#include "display.hpp"
#include "config.hpp"



// Pins for the quadrature encoder and encoder switch.
#define ENC_SW 2
#define ENC_A A3
#define ENC_B A4

// Pins for the PCB-mounted LEDs. Not applicable to production units.
#define PCBLED1 3
#define PCBLED2 4


// Maximum counter value. Values above 9999 and below 1 are not supported.
#define MAX_COUNTER 9999

// Pins for the stepper driver. ENA can be left unconnected.
#define STEP A0
#define DIR A1
#define ENA A2



AccelStepper stepper(AccelStepper::DRIVER, STEP, DIR);

const uint64_t pulsesPerRev = STEPS_PER_REV * INDEXING_HEAD_GEAR_RATIO;
const long maxPulsesPerSec = (MAX_SPEED / 60) * STEPS_PER_REV;
const long maxAcceleration = (MAX_ACCEL / 60) * STEPS_PER_REV;

char previousEncoderA = LOW;
char previousEncoderB = LOW;
char previousEncoderSwitch = HIGH; // switch has pull up resistor, so the default value is HIGH

void setup() {
	pinMode(PCBLED1, OUTPUT);
	pinMode(PCBLED2, OUTPUT);

	pinMode(STEP, OUTPUT);
	pinMode(DIR, OUTPUT);
	// pinMode(ENA, OUTPUT); // not needed

	pinMode(ENC_SW, INPUT_PULLUP);
	pinMode(ENC_A, INPUT_PULLUP);
	pinMode(ENC_B, INPUT_PULLUP);

	setupDisplay();

	Serial.begin(9600);
	Serial.println("Starting");
	setDisplayText("HEL0");
	delay(1000);
	setDisplayText("    ");

	Serial.println("Configuring stepper driver pins");
	stepper.setEnablePin(ENA);
	stepper.setMaxSpeed(maxPulsesPerSec);
	stepper.setAcceleration(maxAcceleration);

	Serial.println("Configuring encoder");
	previousEncoderA = digitalRead(ENC_A);
	previousEncoderB = digitalRead(ENC_B);
	previousEncoderSwitch = digitalRead(ENC_SW);
}

int counter = 1;
uint64_t pulsesPerDivision = 0;

enum DeviceState {
	DIVISION_SELECT,
	WAIT_FOR_DRIVE,
	DRIVE
};

DeviceState deviceState = DIVISION_SELECT;

void divisionSelect(){
	// Read Encoder
	char encoderA = digitalRead(ENC_A);
	char encoderB = digitalRead(ENC_B);

	// Turn encoder state into counter value
	if(encoderA == HIGH && previousEncoderA == LOW) {
		if(encoderB == HIGH) {
			counter--;
		} else {
			counter++;
		}
	} else if(encoderA == LOW && previousEncoderA == HIGH) {
		if(encoderB == HIGH) {
			counter++;
		} else {
			counter--;
		}
	}

	previousEncoderA = encoderA;
	previousEncoderB = encoderB;

	// Limit counter value to range(0, MAX_COUNTER)
	counter = min(MAX_COUNTER, max(1, counter));
	
	// Display counter value on display
	setDisplayNumber(counter);

	// Wait for encoder switch to be pushed to confirm counter value and go to WAIT_FOR_DRIVE state
	if(digitalRead(ENC_SW) == LOW && previousEncoderSwitch == HIGH) {
		setDisplayText(" Loc");
		delay(1000);
		setDisplayText("    ");
		setDisplayNumber(counter);
		pulsesPerDivision = pulsesPerRev / counter;
		deviceState = WAIT_FOR_DRIVE;
		previousEncoderSwitch = LOW;
	}
}

void waitForDrive(){
	// Wait for button push
	char currentEncoderSwitch = digitalRead(ENC_SW);

	// If rotary encoder switch is pushed, go to DRIVE state and drive to target position
	if(currentEncoderSwitch == LOW) {
		deviceState = DRIVE;
		delay(10);
		Serial.print("Schritte: ");
		Serial.println((long)pulsesPerDivision);
		Serial.print("MSB: ");
		Serial.println((long)(pulsesPerDivision >> 32));
		digitalWrite(PCBLED1, HIGH);
		stepper.move(pulsesPerDivision);
	}
	previousEncoderSwitch = currentEncoderSwitch;
}

void drive(){
	// send pulses to led/driver
	stepper.run();

	if(stepper.currentPosition() == stepper.targetPosition()) {
		deviceState = WAIT_FOR_DRIVE;
		digitalWrite(PCBLED1, LOW);
		Serial.println("Position erreicht.");
	}
}

void loop() {
	// Main state machine
	switch (deviceState)
	{
	case DIVISION_SELECT:
		divisionSelect();
		break;
	case WAIT_FOR_DRIVE:
		waitForDrive();
		break;
	case DRIVE:
		drive();
		break;
	
	default:
		break;
	}
}
