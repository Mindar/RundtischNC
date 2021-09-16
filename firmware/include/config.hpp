#pragma once

// The gear reduction of the indexing head.
// Keep the "L" after the number and only use integer numbers
#define INDEXING_HEAD_GEAR_RATIO 90L

// The microstep setting of the stepper driver.
// Keep the "L" after the number and only use integer numbers
#define STEPS_PER_REV 400L




// ATTENTION!
// You probably don't need to change the following settings,
// if you do, make sure your motor still has enough torque
// and doesn't stall!

// Maximum Motor RPM
#define MAX_SPEED 90 * 5

// Max RPM/s acceleration
#define MAX_ACCEL 90