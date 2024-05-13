package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class LEDTopSubsystem extends SubsystemBase {
  // private final AddressableLEDBufferView view;

  public LEDTopSubsystem(RobotSignalsLEDbufferLEDSubsystem led) {
    // view = led.createView(...);
  }

  public Command runPattern(LEDPattern pattern) {
    return run(() -> pattern.applyTo());
  }
}
