#include "WPILib.h"

/**
 * This sample program shows how to control a motor using a joystick. In the operator
 * control part of the program, the joystick is read and the value is written to the motor.
 * An Encoder is then used to read the total distance that the motor has turned and
 *   to display it on the SmartDashboard.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs as range from
 * -1 to 1 making it easy to work together. The program also delays a short time in the loop
 * to allow other threads to run. This is generally a good idea, especially since the joystick
 * values are only transmitted from the Driver Station once every 20ms.
 */
class Robot : public SampleRobot {
	Joystick m_stick;

	// The motor to control with the Joystick.
	// This uses a Talon speed controller; use the Victor or Jaguar classes for
	//   other speed controllers.
	Talon m_motor;

	// update every 0.005 seconds/5 milliseconds.
	double kUpdatePeriod = 0.005;

	Encoder m_encoder;

public:
	Robot() :
			m_stick(0), // Initialize Joystick on port 0.
			m_motor(2), // Initialize the Talon on channel 0.
			m_encoder(1, 2) // Iniitialize encoder connected on DIO ports 1 and 2.
	{
		// Distance will give total rotations of the motor, assuming a 360 count encoder.
		m_encoder.SetDistancePerPulse(1.0 / 360.0);
	}

	/**
	 * Runs the motor from the output of a Joystick.
	 * Simultaneously displays encoder values on the SmartDashboard.
	 */
	void OperatorControl() {
		while (IsOperatorControl()) {
			// Set the motor controller's output.
			// This takes a number from -1 (100% speed in reverse) to +1 (100% speed forwards).
			m_motor.Set(m_stick.GetY());

			// Display the total displacement of the encoder, in rotations.
			SmartDashboard::PutNumber("Encoder Distance", m_encoder.GetDistance());

			Wait(kUpdatePeriod); // Wait 5ms for the next update.
		}
	}
};

START_ROBOT_CLASS(Robot);
