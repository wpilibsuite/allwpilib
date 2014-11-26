package $package;


import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;

/**
 * This is a short sample program demonstrating how to use the basic throttle
 * mode of the new CAN Talon. As of when this was made, the basic throttle mode
 * is all that is supported.
 */
public class Robot extends SampleRobot {

  CANTalon motor;

	private final double k_updatePeriod = 0.005; // update every 0.005 seconds/5 milliseconds (200Hz)

  public Robot() {
      motor = new CANTalon(1); // Initialize the CanTalonSRX on device 1.
  }

  /**
    * Runs the motor from a joystick.
    */
  public void operatorControl() {
    motor.enableControl();
    while (isOperatorControl() && isEnabled()) {
      // Set the motor's output to half power.
      // This takes a number from -1 (100% speed in reverse) to +1 (100% speed
      // going forward)
      motor.set(0.5);

      Timer.delay(k_updatePeriod);  // wait 5ms to the next update
    }
    motor.disable();
  }
}
