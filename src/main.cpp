/*
 * Copyright (c) 2019-2022, Michael Gummere.
 * All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "main.h"
#include "opfunctions.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// ARMS init
	arms::chassis::init();
	arms::pid::init();
	// arms::odom::init();

	// Set display
	arms::selector::init();
	// display();

	// Reset IMU and start display update task
	// imuDisplay();
	imu_sensor.reset();
	// pros::Task displayUpdateTask(displayUpdate);
	pros::Task task{[=] {
		pros::delay(200);
		master.print(1, 1, "Speed: %d", leftMtr.get_actual_velocity());
	}};

	// Set brakes on to active bold
	rightLift.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	leftLift.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	// clawM.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// winchM.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
	arms::selector::init();
}
