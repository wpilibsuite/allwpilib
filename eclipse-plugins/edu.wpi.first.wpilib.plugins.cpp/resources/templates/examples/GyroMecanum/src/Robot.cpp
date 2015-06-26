#include "WPILib.h"

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to maintian
 * rotation vectors in relation to the starting orientation of the robot (field-oriented controls).
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public SampleRobot {
	Joystick *joystick;
	RobotDrive *myRobot;
	Gyro *gyro;

	//channels for motors
	const int leftMotorChannel = 1;
	const int rightMotorChannel = 0;
	const int leftRearMotorChannel = 3;
	const int rightRearMotorChannel = 2;

	const int gyroChannel = 0; //analog input

	//gyro calibration constant, may need to be adjusted so that a gyro value of 360
	//equals 360 degrees
	const double voltsPerDegreePerSecond = .0128;

public:
	Robot() :
			SampleRobot() {
		//make objects for drive train, joystick, and gyro
		joystick = new Joystick(0);
		myRobot = new RobotDrive(new CANTalon(leftMotorChannel),
				new CANTalon(leftRearMotorChannel),
				new CANTalon(rightMotorChannel),
				new CANTalon(rightRearMotorChannel));
		myRobot->SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);// invert the left side motors
		myRobot->SetInvertedMotor(RobotDrive::kRearLeftMotor, true);// you may need to change or remove this to match your robot

		gyro = new Gyro(gyroChannel);
	}

	/**
	 * Runs during autonomous.
	 */
	void Autonomous() {

	}

	/**
	 * Runs the motors with arcade steering.
	 */
	void OperatorControl() {
		gyro->SetSensitivity(voltsPerDegreePerSecond); //calibrate gyro to have the value equal to degrees
		while (IsOperatorControl() && IsEnabled()) {
			myRobot->MecanumDrive_Cartesian(joystick->GetX(), joystick->GetY(),
					joystick->GetZ(), gyro->GetAngle());
			Wait(0.005);	// wait 5ms to avoid hogging CPU cycles
		}
	}

	/**
	 * Runs during test mode.
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(Robot);
