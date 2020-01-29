#include "main.h"

// Distance in inches, Speed in rpms, Wait in milliseconds
void drive(double distance, int speed, int wait) {

    double wheel_radius = 2.065; //inches
    double const pi = 3.14159265;
    //1 rotation will move the robot 2*pi*wheel_radius inches
    double rotations = distance/(pi*2*wheel_radius);
    left_mtr.move_relative(rotations, speed);
    right_mtr.move_relative(rotations, speed);

    rotations += left_mtr.get_position();

    while (!((left_mtr.get_position() < rotations + 0.5) && (left_mtr.get_position() > rotations - 0.5 ))) {
        pros::delay(5);
	}

    pros::delay(wait);

}

// Degrees of turn.  Speed is RPM, Wait is milliseconds
void turn(double degrees, int speed, int wait) {

    double one_rotation_turn_degrees = 115; //customize to your robot
    double rotations = degrees/one_rotation_turn_degrees;
    left_mtr.move_relative(rotations, speed);
    right_mtr.move_relative(-rotations, speed);

    rotations += left_mtr.get_position();

    while (!((left_mtr.get_position() < rotations + 0.5) && (left_mtr.get_position() > rotations -0.5 ))) {
        pros::delay(20);
    }
    
    pros::delay(wait);

}

// grip the claw
void grab(double degrees, int speed, int wait) {

    double one_rotation_turn_degrees = 115; //customize to your robot
    double rotations = degrees/one_rotation_turn_degrees;
    claw_mtr.move_relative(rotations, speed);

    rotations += claw_mtr.get_position();

    while (!((claw_mtr.get_position() < rotations + 0.5) && (claw_mtr.get_position() > rotations -0.5 ))) {
        pros::delay(20);
    }
    
    pros::delay(wait);

}

// lift the claw
void lift(double degrees, int speed, int wait) {

    double one_rotation_turn_degrees = 115; //customize to your robot
    double rotations = degrees/one_rotation_turn_degrees;
    arm_mtr.move_relative(rotations, speed);

    rotations += arm_mtr.get_position();

    while (!((arm_mtr.get_position() < rotations + 0.5) && (arm_mtr.get_position() > rotations -0.5 ))) {
        pros::delay(20);
    }
    
    pros::delay(wait);

}