#include "WPILib.h"

/**
 * Sample program displaying the value of a quadrature encoder on the SmartDashboard.
 * Quadrature Encoders are digital sensors which can detect the amount the encoder
 *   has rotated since starting as well as the direction in which the encoder
 *   shaft is rotating. However, encoders can not tell you the absolute
 *   position of the encoder shaft (ie, it considers where it starts to be the
 *   zero position, no matter where it starts), and so can only tell you how
 *   much the encoder has rotated since starting.
 * Depending on the precision of an encoder, it will have fewer or greater
 *   ticks per revolution; the number of ticks per revolution will affect the
 *   conversion between ticks and distance, as specified by DistancePerPulse.
 * One of the most common uses of encoders is in the drivetrain, so that the
 *   distance that the robot drives can be precisely controlled during the
 *   autonomous mode.
 */
class Robot: public SampleRobot
{
	Encoder m_encoder;

	/**
	 * Time to wait between updating SmartDashboard values.
	 * It is generally a good idea to stick a short wait in these loops
	 *   to avoid hogging CPU power, especially as there will be no
	 *   perceivable difference in the SmartDashboard display.
	 */
	const double kUpdatePeriod = 0.005; // 5milliseconds / 0.005 seconds.

public:
	/**
	 * The Encoder object is constructed with 4 parameters, the last two being optional.
	 * The first two parameters (1, 2 in this case) refer to the ports on the
	 *   roboRIO which the encoder uses. Because a quadrature encoder has
	 *   two signal wires, the signal from two DIO ports on the roboRIO are used.
	 * The third (optional) parameter is a boolean which defaults to false.
	 *   If you set this parameter to true, the direction of the encoder will
	 *   be reversed, in case it makes more sense mechanically.
	 * The final (optional) parameter specifies encoding rate (k1X, k2X, or k4X)
	 *   and defaults to k4X. Faster (k4X) encoding gives greater positional
	 *   precision but more noise in the rate.
	 */
	Robot() : m_encoder(1, 2, false, Encoder::k4X)
	{
		// Defines the number of samples to average when determining the rate.
		// On a quadrature encoder, values range from 1-255; larger values
		//   result in smoother but potentially less accurate rates than lower values.
		m_encoder.SetSamplesToAverage(5);

		// Defines how far the mechanism attached to the encoder moves per pulse.
		// In this case, we assume that a 360 count encoder is directly attached
		//   to a 3 inch diameter (1.5inch radius) wheel, and that we want to
		//   measure distance in inches.
		m_encoder.SetDistancePerPulse(1.0 / 360.0 * 2.0 * 3.1415 * 1.5);

		// Defines the lowest rate at which the encoder will not be considered
		//   stopped, for the purposes of the GetStopped() method.
		// Units are in distance / second, where distance refers to the units
		//   of distance that you are using, in this case inches.
		m_encoder.SetMinRate(1.0);
	}

	/**
	 * Retrieve various information from the encoder and display it
	 *   on the SmartDashboard.
	 */
	void OperatorControl()
	{
		while (IsOperatorControl() && IsEnabled())
		{
			// Retrieve the net displacement of the Encoder since the lsat Reset.
			SmartDashboard::PutNumber("Encoder Distance", m_encoder.GetDistance());

			// Retrieve the current rate of the encoder.
			SmartDashboard::PutNumber("Encoder Rate", m_encoder.GetRate());

			Wait(kUpdatePeriod); // Wait a short bit before updating again.
		}
	}
};

START_ROBOT_CLASS(Robot)
