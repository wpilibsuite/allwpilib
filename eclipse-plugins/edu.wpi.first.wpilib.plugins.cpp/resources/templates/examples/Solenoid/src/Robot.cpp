#include "WPILib.h"

/**
 * This is a sample program showing the use of the solenoid classes during
 *   operator control.
 * Three buttons from a joystick will be used to control two solenoids:
 *   One button to control the position of a single solenoid and the other
 *   two buttons to control a double solenoid.
 * Single solenoids can either be on or off, such that the air diverted through
 *   them goes through either one channel or the other.
 * Double solenoids have three states: Off, Forward, and Reverse. Forward and
 *   Reverse divert the air through the two channels and correspond to the
 *   on and off of a single solenoid, but a double solenoid can also be "off",
 *   where both channels are diverted to exhaust such that there is no pressure
 *   in either channel.
 * Additionally, double solenoids take up two channels on your PCM whereas
 *   single solenoids only take a single channel.
 * During Operator Control, the loop waits for a brief time before continuing
 *   in order to allow other threads to run. This is generally a good idea,
 *   especially as joystick values are only received every 20ms.
 */
class Robot: public SampleRobot
{
	// Joystick with buttons to control solenoids with.
	Joystick m_stick;
	// Solenoids to control with the joystick.
	// Solenoid corresponds to a single solenoid.
	Solenoid m_solenoid;
	// DoubleSolenoid corresponds to a double solenoid.
	DoubleSolenoid m_doubleSolenoid;

	// Update every 5milliseconds/0.005 seconds.
	const double kUpdatePeriod = 0.005;

	// Numbers of the buttons to use for triggering the solenoids.
	const int kSolenoidButton = 1;
	const int kDoubleSolenoidForward = 2;
	const int kDoubleSolenoidReverse = 3;

public:
	Robot() :
			m_stick(0), // Use joystick on port 0.
			m_solenoid(0), // Use solenoid on channel 0.
			// Use double solenoid with Forward Channel of 1 and Reverse of 2.
			m_doubleSolenoid(1, 2)
	{
	}

	/**
	 * Sets the solenoids from the position of joystick buttons.
	 */
	void OperatorControl()
	{
		while (IsOperatorControl() && IsEnabled())
		{
			// The output of GetRawButton is true/false depending on whether
			//   the button is pressed; Set takes a boolean for for whether to
			//   use the default (false) channel or the other (true).
			m_solenoid.Set(m_stick.GetRawButton(kSolenoidButton));

			// In order to set the double solenoid, we will say that if neither
			//   button is pressed, it is off, if just one button is pressed,
			//   set the solenoid to correspond to that button, and if both
			//   are pressed, set the solenoid to Forwards.
			if (m_stick.GetRawButton(kDoubleSolenoidForward))
				m_doubleSolenoid.Set(DoubleSolenoid::kForward);
			else if (m_stick.GetRawButton(kDoubleSolenoidReverse))
				m_doubleSolenoid.Set(DoubleSolenoid::kReverse);
			else
				m_doubleSolenoid.Set(DoubleSolenoid::kOff);

			Wait(kUpdatePeriod);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(Robot);
