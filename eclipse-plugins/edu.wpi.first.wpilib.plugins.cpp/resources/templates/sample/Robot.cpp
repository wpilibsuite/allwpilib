#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotDrive class.
 * The SampleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public SampleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick

public:
	Robot() :
			myRobot(0, 1),	// these must be initialized in the same order
			stick(0)		// as they are declared above.
	{
		myRobot.SetExpiration(0.1);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous()
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering.
	 */
	void OperatorControl()
	{
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl() && IsEnabled())
		{
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test()
	{
	}
};

START_ROBOT_CLASS(Robot);
