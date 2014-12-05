package $package;


import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;

/**
 * This is a short sample program demonstrating how to use the Talon SRX over
 * CAN to run a closed-loop PID controller with an analog potentiometer.
 */
public class Robot extends SampleRobot {

  CANTalon motor;

  public Robot() {
      motor = new CANTalon(1); // Initialize the CanTalonSRX on device 1.

      // This sets the mode of the m_motor. The options are:
      // PercentVbus: basic throttle; no closed-loop.
      // Current: Runs the motor with the specified current if possible.
      // Speed: Runs a PID control loop to keep the motor going at a constant
      //   speed using the specified sensor.
      // Position: Runs a PID control loop to move the motor to a specified move
      //   the motor to a specified sensor position.
      // Voltage: Runs the m_motor at a constant voltage, if possible.
      // Follower: The m_motor will run at the same throttle as the specified
      //   other talon.
      motor.changeControlMode(CANTalon.ControlMode.Position);
      // This command allows you to specify which feedback device to use when doing
      // closed-loop control. The options are:
      // AnalogPot: Basic analog potentiometer
      // QuadEncoder: Quadrature Encoder
      // AnalogEncoder: Analog Encoder
      // EncRising: Counts the rising edges of the QuadA pin (allows use of a
      //   non-quadrature encoder)
      // EncFalling: Same as EncRising, but counts on falling edges.
      motor.setFeedbackDevice(CANTalon.FeedbackDevice.AnalogPot);
      // This sets the basic P, I , and D values (F, Izone, and rampRate can also
      //   be set, but are ignored here).
      // These must all be positive floating point numbers (reverseSensor will
      //   multiply the sensor values by negative one in case your sensor is flipped
      //   relative to your motor).
      // These values are in units of throttle / sensor_units where throttle ranges
      //   from -1023 to +1023 and sensor units are from 0 - 1023 for analog
      //   potentiometers, encoder ticks for encoders, and position / 10ms for
      //   speeds.
      motor.setPID(1.0, 0.0, 0.0);
  }

  public void operatorControl() {
    while (isOperatorControl() && isEnabled()) {
      // In closed loop mode, this sets the goal in the units mentioned above.
      // Since we are using an analog potentiometer, this will try to go to
      //   the middle of the potentiometer range.
			motor.set(512);

      Timer.delay(5.0);
    }
  }
}
