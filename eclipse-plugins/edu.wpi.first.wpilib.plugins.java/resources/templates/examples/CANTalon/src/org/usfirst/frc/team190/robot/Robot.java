package $package;


import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;

/**
 * This is a short sample program demonstrating how to use the basic throttle
 * mode of the new CAN Talon.
 */
public class Robot extends SampleRobot {

  CANTalon motor;

  public Robot() {
      motor = new CANTalon(1); // Initialize the CanTalonSRX on device 1.
  }

  /**
    * Runs the motor.
    */
  public void operatorControl() {
    while (isOperatorControl() && isEnabled()) {
      // Set the motor's output to half power.
      // This takes a number from -1 (100% speed in reverse) to +1 (100% speed
      // going forward)
      motor.set(0.5);

      Timer.delay(0.01);  // Note that the CANTalon only receives updates every
                          // 10ms, so updating more quickly would not gain you
                          // anything.
    }
    motor.disable();
  }
}
