#include "WPILib.h"

/**
 * This is a sample program demonstrating how to use an ultrasonic sensor and proportional
 * control to maintain a set distance from an object.
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public SampleRobot {
	AnalogInput *ultrasonic; //ultrasonic sensor
	RobotDrive *myRobot;

public:
	const int ultrasonicChannel = 3; //analog input pin

	//channels for motors
	const int leftMotorChannel = 1;
	const int rightMotorChannel = 0;
	const int leftRearMotorChannel = 3;
	const int rightRearMotorChannel = 2;

	int holdDistance = 12; //distance in inches the robot wants to stay from an object
	const double valueToInches = 0.125; //factor to convert sensor values to a distance in inches
	const double pGain = 0.05; //proportional speed constant


	Robot() :
			SampleRobot() {
		//make objects for the sensor and drive train
		ultrasonic = new AnalogInput(ultrasonicChannel);
		myRobot = new RobotDrive(new CANTalon(leftMotorChannel),
				new CANTalon(leftRearMotorChannel),
				new CANTalon(rightMotorChannel),
				new CANTalon(rightRearMotorChannel));
	}

	/**
	 * Runs during autonomous.
	 */
	void Autonomous() {

	}

	/**
	 *  Tells the robot to drive to a set distance (in inches) from an object using
	 *  proportional control.
	 */
	void OperatorControl() {

		double currentDistance; //distance measured from the ultrasonic sensor values
		double currentSpeed; //speed to set the drive train motors

		while (IsOperatorControl() && IsEnabled()) {
			currentDistance = ultrasonic->GetValue() * valueToInches; //sensor returns a value from 0-4095 that is scaled to inches
			currentSpeed = (holdDistance - currentDistance) * pGain; //convert distance error to a motor speed
			myRobot->Drive(currentSpeed, 0); //drive robot
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(Robot);
