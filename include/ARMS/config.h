#ifndef _ARMS_CONFIG_H_
#define _ARMS_CONFIG_H_

#include "ARMS/api.h"
#include "ARMS/odom.h"

namespace arms {

// Debug
#define ODOM_DEBUG 0

// Negative numbers mean reversed motor
#define LEFT_MOTORS -2, 4, -5
#define RIGHT_MOTORS 6, -7, 8
#define GEARSET pros::E_MOTOR_GEAR_600 // RPM of chassis motors

// Ticks per inch
#define TPI 46.5        // Encoder ticks per inch of forward robot movement
#define MIDDLE_TPI 33 // Ticks per inch for the middle wheel

// Tracking wheel distances
#define TRACK_WIDTH 3.5 // The distance between left and right wheels (or tracker wheels)
#define MIDDLE_DISTANCE 3.5 // Distance from middle wheel to the robot turning center

// Sensors
#define IMU_PORT 1                                // Port 0 for disabled
#define ENCODER_PORTS 0, 0, 20                // Port 0 for disabled,
#define EXPANDER_PORT 0                           // Port 0 for disabled
#define ENCODER_TYPE arms::odom::ENCODER_ROTATION // The type of encoders

// Movement tuning
#define SLEW_STEP 8             // Smaller number = more slew
#define LINEAR_EXIT_ERROR 1     // default exit distance for linear movements
#define ANGULAR_EXIT_ERROR 1    // default exit distance for angular movements
#define SETTLE_THRESH_LINEAR .5 // amount of linear movement for settling
#define SETTLE_THRESH_ANGULAR 1 // amount of angular movement for settling
#define SETTLE_TIME 250         // amount of time to count as settled

#define LINEAR_KP 5
#define LINEAR_KI 0
#define LINEAR_KD 1

#define TRACKING_KP 60 // point tracking turning strength

#define ANGULAR_KP 1
#define ANGULAR_KI 0
#define ANGULAR_KD 0

#define MIN_ERROR 5 // Minimum distance to target before angular componenet is disabled
#define LEAD_PCT .6 // Go-to-pose lead distance ratio (0-1)

// Auton selector configuration constants
#define AUTONS "Short", "Long", "SAWP", "LAWP", "Sit." // Names of autonomi, up to 10
#define HUE 0     // Color of theme from 0-359(H part of HSV)
#define DEFAULT -1 // Default auton selected

inline const char* autons[] = {AUTONS, ""};

// Initializer
inline void init() {
	chassis::init({LEFT_MOTORS}, {RIGHT_MOTORS}, GEARSET, SLEW_STEP, LINEAR_EXIT_ERROR,
	              ANGULAR_EXIT_ERROR, SETTLE_THRESH_LINEAR, SETTLE_THRESH_ANGULAR,
	              SETTLE_TIME);

	odom::init(ODOM_DEBUG, ENCODER_TYPE, {ENCODER_PORTS}, EXPANDER_PORT, IMU_PORT,
	           TRACK_WIDTH, MIDDLE_DISTANCE, TPI, MIDDLE_TPI);

	pid::init(LINEAR_KP, LINEAR_KI, LINEAR_KD, ANGULAR_KP, ANGULAR_KI, ANGULAR_KD,
	          TRACKING_KP, MIN_ERROR, LEAD_PCT);

	const char* b[] = {AUTONS, ""};
	selector::init(HUE, DEFAULT, b);
}

} // namespace arms

#endif
