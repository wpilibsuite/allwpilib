#include "WPILib.h"

/**
 * This is a quick sample program to show how to use the new Talon SRX over CAN.
 * This particular sample demonstrates running a basic PID control loop with an
 *   analog potentiometer.
 *
 */
class Robot : public SampleRobot {
	CANTalon m_motor;

public:
 Robot()
     : m_motor(1)   // Initialize the Talon as device 1. Use the roboRIO web
                    // interface to change the device number on the m_motors.
 {
   // This sets the mode of the m_motor. The options are:
   // kPercentVbus: basic throttle; no closed-loop.
   // kCurrent: Runs the motor with the specified current if possible.
   // kSpeed: Runs a PID control loop to keep the motor going at a constant
   //   speed using the specified sensor.
   // kPosition: Runs a PID control loop to move the motor to a specified move
   //   the motor to a specified sensor position.
   // kVoltage: Runs the m_motor at a constant voltage, if possible.
   // kFollower: The m_motor will run at the same throttle as the specified other talon.
   m_motor.SetControlMode(CANSpeedController::kPosition);
   // This command allows you to specify which feedback device to use when doing
   // closed-loop control. The options are:
   // AnalogPot: Basic analog potentiometer
   // QuadEncoder: Quadrature Encoder
   // AnalogEncoder: Analog Encoder
   // EncRising: Counts the rising edges of the QuadA pin (allows use of a
   //   non-quadrature encoder)
   // EncFalling: Same as EncRising, but counts on falling edges.
   m_motor.SetFeedbackDevice(CANTalon::AnalogPot);
   // This sets the basic P, I , and D values (F, Izone, and rampRate can also
   //   be set, but are ignored here).
   // These must all be positive floating point numbers (SetSensorDirection will
   //   multiply the sensor values by negative one in case your sensor is flipped
   //   relative to your motor).
   // These values are in units of throttle / sensor_units where throttle ranges
   //   from -1023 to +1023 and sensor units are from 0 - 1023 for analog
   //   potentiometers, encoder ticks for encoders, and position / 10ms for
   //   speeds.
   m_motor.SetPID(1.0, 0.0, 0.0);
 }

	/**
	 * Runs the motor from the output of a Joystick.
	 */
	void OperatorControl() {
		while (IsOperatorControl() && IsEnabled()) {
      // In closed loop mode, this sets the goal in the units mentioned above.
      // Since we are using an analog potentiometer, this will try to go to
      //   the middle of the potentiometer range.
			m_motor.Set(512);

			Wait(5.0);
		}
	}
};

START_ROBOT_CLASS(Robot)
