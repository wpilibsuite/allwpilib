#include "WPILib.h"

/**
 * This is a sample program to demonstrate the use of a soft potentiometer and proportional
 * control to reach and maintain position setpoints on an elevator mechanism. A joystick
 * button is used to switch elevator setpoints.
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public SampleRobot
{

		const int potChannel = 1; //analog input pin
	    const int motorChannel = 7; //PWM channel
	    const int joystickChannel = 0; //usb number in DriverStation
	    const int buttonNumber = 4; //joystick button

	    const double pGain = 1.0; //proportional speed constant
	    double motorSpeed;
	    double currentPosition; //sensor voltage reading corresponding to current elevator position

	    AnalogInput potentiometer;
	    Victor elevatorMotor;
	    Joystick joystick;

public:
	Robot() :
			potentiometer(potChannel), elevatorMotor(motorChannel),
      joystick(joystickChannel) {}

	/**
	 * Runs during autonomous.
	 */
	void Autonomous()
	{

	}

	/**
	 *
	 */
	void OperatorControl() {
		bool buttonState;
		bool prevButtonState = false;

		int index = 0; //setpoint array index
	    double currentSetpoint; //holds desired setpoint
	    const int size = 3; //number of setpoints
	    const double setpoints[size] = {1.0, 2.6, 4.3}; //bottom, middle, and top elevator setpoints
		currentSetpoint = setpoints[0]; //set to first setpoint

		while (IsOperatorControl() && IsEnabled()) {
			buttonState = joystick.GetRawButton(buttonNumber); //check if button is pressed

			//if button has been pressed and released once
			if (buttonState && !prevButtonState) {
				index = (index + 1) % size; //increment set point, reset if at maximum
				currentSetpoint = setpoints[index]; //set setpoint
			}
			prevButtonState = buttonState; //record previous button state

			currentPosition = potentiometer.GetAverageVoltage(); //get position value
			motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
			elevatorMotor.Set(motorSpeed); //drive elevator motor
		}
	}

	/**
	 * Runs during test mode.
	 */
	void Test()
	{

	}
};

START_ROBOT_CLASS(Robot)
