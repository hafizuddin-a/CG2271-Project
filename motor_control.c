#include "motor_control.h"

void forward(void) {
	// Right motor
	TPM1_C0V = FULLSPEED;
	TPM1_C1V = 0;
	// Left motor
	TPM2_C0V = FULLSPEED;
	TPM2_C1V = 0;
}

void forwardRight(void) {
	// Right motors
	TPM1_C0V = TURNINGSPEED;
	TPM1_C1V = 0;
	// Left motors
	TPM2_C0V = FULLSPEED;
	TPM2_C1V = 0;
}

void forwardLeft(void) {
	// Right motors
	TPM1_C0V = FULLSPEED;
	TPM1_C1V = 0;
	// Left motors
	TPM2_C0V = TURNINGSPEED;
	TPM2_C1V = 0;
}

void backward(void) {
	//right motors
	TPM1_C0V = 0;
	TPM1_C1V = FULLSPEED;
	//left motors
	TPM2_C0V = 0;
	TPM2_C1V = FULLSPEED;
}

void backwardRight(void) {
	// Right motors
	TPM1_C0V = 0;
	TPM1_C1V = TURNINGSPEED;
	// Left motors
	TPM2_C0V = 0;
	TPM2_C1V = FULLSPEED;
}

void backwardLeft(void) {
	// Right motors
	TPM1_C0V = 0;
	TPM1_C1V = FULLSPEED;
	// Left motors
	TPM2_C0V = 0;
	TPM2_C1V = TURNINGSPEED;
}

void clockwise(void) {
	// Right motors
	TPM1_C0V = 0;
	TPM1_C1V = FULLSPEED;
	// Left motors
	TPM2_C0V = FULLSPEED;
	TPM2_C1V = 0;
}

void antiClockwise(void) {
	// Right motors
	TPM1_C0V = FULLSPEED;
	TPM1_C1V = 0;
	// Left motors
	TPM2_C0V = 0;
	TPM2_C1V = FULLSPEED;
}

void stop(void) {
	// Right motors
	TPM1_C0V = 0;
	TPM1_C1V = 0;
	// Left motors
	TPM2_C0V = 0;
	TPM2_C1V = 0;
}
