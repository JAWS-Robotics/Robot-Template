/*
  Copyright (c) 2019-2023 7701F

  This software is provided 'as-is', without any express or implied warranty. In no event
  will the authors be held liable for any damages arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to the
  following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you
  wrote the original software. If you use this software in a product, an acknowledgment
  (see the following) in the product documentation is required.

  Portions Copyright (c) 2019-2023 7701F

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.
*/
#include "7701.hpp"

/// @brief Smart boy motor brake, additional parameter to set brake type
/// @param on true to brake, false to coast
/// @param type 0 to brake, 1 to hold
void prosBrake(bool on, int type) {
	if (on == true) {
		// pros::E_MOTOR_BRAKE_HOLD if type is set to 0
		if (type == 0) {
			if (arms::chassis::rightMotors->get_brake_modes() != std::vector<pros::motor_brake_mode_e>(4, pros::E_MOTOR_BRAKE_HOLD)) {
				arms::chassis::leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
				arms::chassis::rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
			}
		}

		// pros::E_MOTOR_BRAKE_BRAKE if type is set to 1
		if (type == 1) {
			if (arms::chassis::rightMotors->get_brake_modes() != std::vector<pros::motor_brake_mode_e>(4, pros::E_MOTOR_BRAKE_BRAKE)) {
				arms::chassis::leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_BRAKE);
				arms::chassis::rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_BRAKE);
			}
		}
	} else if (on == false) {
		// coasting, ie, no braking
		if (arms::chassis::rightMotors->get_brake_modes() != std::vector<pros::motor_brake_mode_e>(4, pros::E_MOTOR_BRAKE_COAST)) {
			arms::chassis::leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_COAST);
			arms::chassis::rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_COAST);
		}
	}
}

/// @brief Smart boy motor brake
/// @param on true to brake, false to coast
void prosBrake(bool on) {
	if (on == true) {
		// actively holds position
		if (arms::chassis::rightMotors->get_brake_modes() != std::vector<pros::motor_brake_mode_e>(4, pros::E_MOTOR_BRAKE_HOLD)) {
			arms::chassis::leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
			arms::chassis::rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
		}
	} else if (on == false) {
		// coasting, ie, no braking
		if (arms::chassis::rightMotors->get_brake_modes() != std::vector<pros::motor_brake_mode_e>(4, pros::E_MOTOR_BRAKE_COAST)) {
			arms::chassis::leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_COAST);
			arms::chassis::rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_COAST);
		}
	}
}

// Update inteval (in mS) for the flywheel control loop
#define FW_LOOP_SPEED 20

// Maximum power we want to send to the flywheel motors
#define FW_MAX_POWER 210

namespace deFenestration::Flywheel {

/// @brief Flywheel motor velocity to voltage conversion
/// @param value motor velocity in RPM
void FwMotorSet(int value) {
	int x = (value * 12000) / 210;
	fw.move_voltage(x);
}

/// @brief Outward facing function to set flywheel velocity
/// @param velocity target velocity (motor rpm)
/// @param predicted_drive predicted open loop drive value
void FwVelocitySet(int velocity, float predicted_drive) {
	// set target_velocity velocity (motor rpm)
	target_velocity = velocity;

	// Set error so zero crossing is correctly detected
	current_error = target_velocity - motor_velocity;
	last_error = current_error;

	// Set predicted open loop drive value
	drive_approx = predicted_drive;
	// Set flag to detect first zero crossing
	first_cross = 1;
	// clear tbh variable
	drive_at_zero = 0;
}

/// @brief Calculate the flywheel velocity
void FwCalculateSpeed() {
	current_time = pros::millis();
	delta_ms = 1000.0 / (current_time - millis_last);

	encoder_counts = fw.get_position();

	// Calculate the change in encoder counts since the last time this function was called
	delta_enc = (encoder_counts - encoder_counts_last);

	// Calculate the motor velocity in RPM
	motor_velocity = (1000.0 / delta_ms) * delta_enc * 60.0 / ticks_per_rev;

	encoder_counts_last = encoder_counts;
	millis_last = current_time;
}

/// @brief Update the velocity tbh controller variables
void FwControlUpdateVelocityTbh() {
	// calculate error in velocity
	// target_velocity is desired velocity
	// current is measured velocity
	current_error = target_velocity - motor_velocity;

	// Calculate new control value
	drive = drive + (current_error * gain);

	// Clip to the range 0 - 1.
	// We are only going forwards
	if (drive > 1)
		drive = 1;
	if (drive < 0)
		drive = 0;

	// Check for zero crossing
	if (current_error != last_error) {
		// First zero crossing after a new set velocity command
		if (first_cross) {
			// Set drive to the open loop approximation
			drive = drive_approx;
			first_cross = 0;
		} else
			drive = 0.5 * (drive + drive_at_zero);

		// Save this drive value in the "tbh" variable
		drive_at_zero = drive;
	}

	// Save last error
	last_error = current_error;
}

/// @brief Flywheel control task
void FwControlTask() {
	// Set the gain
	// gain = 0.0005;
	gain = 0.00025;

	while (true) {
		// Calculate velocity
		deFenestration::Flywheel::FwCalculateSpeed();

		// Do the velocity TBH calculations
		deFenestration::Flywheel::FwControlUpdateVelocityTbh();

		// Scale drive into the range the motors need
		motor_drive = (drive * FW_MAX_POWER) + 0.5;

		// Final Limit of motor values - don't really need this
		if (motor_drive > 210)
			motor_drive = 210;
		if (motor_drive < -200)
			motor_drive = -200;

		// and finally set the motor control value
		deFenestration::Flywheel::FwMotorSet(motor_drive);

		// Run at somewhere between 20 and 50mS
		pros::delay(FW_LOOP_SPEED);
	}
}

} // namespace deFenestration::Flywheel

/// @brief Exponential drive function, uses a cubic function to scale joystick values
/// @param joyVal joystick value
/// @return scaled joystick value
std::int32_t exponentialDrive(std::int32_t joyVal) {
	// return joyVal;
	return pow(joyVal, 3) / 10000;
}

/*
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	// set brake mode
	prosBrake(true, 1);

	// set flywheel velocity to 0
	deFenestration::Flywheel::FwVelocitySet(0, 0.0);
	// set intake velocity to 0
	conveyor.move_velocity(0);

	// set current limit
	// arms::chassis::leftMotors->set_voltage_limit(12000);
	// arms::chassis::rightMotors->set_voltage_limit(12000);

	// set conveyor current limit
	// conveyor.set_current_limit(7000);

	// Run Loop
	while (true) {
		/* Steering */
		int leftJoyStick = (master.get_analog(ANALOG_LEFT_Y) * (double)100 / 127);
		int rightJoyStick = (master.get_analog(ANALOG_RIGHT_X) * (double)100 / 127);

		// Minor deadzone to account for stick drift
		if (abs(leftJoyStick) < 1.5)
			leftJoyStick = 0;
		if (abs(rightJoyStick) < 1.5)
			rightJoyStick = 0;

		/* Brake System
		 * The brake system is a safety feature that prevents the robot from being
		 * punished by other robots. Uses basic logic for toggle button
		 */
		if (master.get_digital_new_press(DIGITAL_B) == 1)
			pbrake = !pbrake;
		prosBrake(pbrake, 1);

		// clang-format off
		// Drive Control
		arms::chassis::arcade(
			exponentialDrive(leftJoyStick),
			exponentialDrive(rightJoyStick)
		);
		// clang-format on

		// Disc Flywheel
		if (master.get_digital_new_press(DIGITAL_A))
			fwON = !fwON;

		// flywheel full speed
		if (master.get_digital_new_press(DIGITAL_L1) && fwON == true) {
			deFenestration::Flywheel::FwVelocitySet(198, 0.81);
		}

		// flywheel 2/3 speed
		if (master.get_digital_new_press(DIGITAL_L2) && fwON == true) {
			deFenestration::Flywheel::FwVelocitySet(120, .6);
		}

		// flywheel off if fwON is false or no button is pressed
		if (fwON == false) {
			deFenestration::Flywheel::FwVelocitySet(0, 0.0);
		}

		// Disc Intake & Roller Mechanism
		if (master.get_digital(DIGITAL_R1)) {
			// intake
			conveyor.move_velocity(600);
		} else if (master.get_digital(DIGITAL_Y)) {
			// outtake
			conveyor.move_velocity(-600);
		} else {
			// stop if neither button is pressed
			conveyor.move_velocity(0);
		}

		// Endgame Piston
		EpistonState = master.get_digital_new_press(DIGITAL_LEFT);
		if (EpistonState == true && EprevPistonState == false) {
			toggleEndgame();
		}
		EprevPistonState = EpistonState;

		// Indexer, launches discs
		if (master.get_digital_new_press(DIGITAL_R2))
			fireDisc();

		/* Autonomous Manual Trigger
		 * If the robot is not connected to competition control
		 * and the button is pressed, the robot will begin the
		 * autonomous routine to allow for easy testing.
		 */
		if (master.get_digital_new_press(DIGITAL_X) && !pros::competition::is_connected())
			autonomous();

		// Lastly, delay
		pros::delay(20);
	}
}
