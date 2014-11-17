package $package;


import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.hal.CanTalonSRX;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;

/**
 * This is a short sample program demonstrating how to use the basic throttle
 * mode of the new CAN Talon. As of when this was made, the basic throttle mode
 * is all that is supported.
 */
public class Robot extends SampleRobot {

  CanTalonSRX motor;

	private final double k_updatePeriod = 0.005; // update every 0.005 seconds/5 milliseconds (200Hz)

  public Robot() {
      motor = new CanTalonSRX(1); // Initialize the CanTalonSRX on device 1.
  }

  /**
    * Runs the motor from a joystick.
    */
  public void operatorControl() {
    motor.SetModeSelect(0);
    while (isOperatorControl() && isEnabled()) {
      // Set the motor's output.
      // This takes a number from -1 (100% speed in reverse) to +1 (100% speed
      // going forward)
      motor.Set(0.5);

      Timer.delay(k_updatePeriod);  // wait 5ms to the next update
    }
    motor.Set(0.0);
  }
}
