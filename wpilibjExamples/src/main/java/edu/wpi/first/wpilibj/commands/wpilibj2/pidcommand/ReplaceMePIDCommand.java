package edu.wpi.first.wpilibj.commands.wpilibj2.pidcommand;

import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj2.command.PIDCommand;

// NOTE:  Consider using this command inline, rather than writing a subclass.  For more
// information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
public class ReplaceMePIDCommand extends PIDCommand {

  public ReplaceMePIDCommand() {
    super(new PIDController(0, 0, 0),
          // This should return the measurement
          () -> 0,
          // This should return the setpoint (can also be a constant)
          () -> 0,
          // This uses the output
          (output) -> {
            // Use the output here
          });
    // Use addRequirements() here to declare subsystem dependencies.
    // Configure additional PID options by calling `getController` here.
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }
}
