package edu.wpi.first.wpilibj.commands.wpilibj2.pidcommand;

import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj2.command.PIDCommand;

public class ReplaceMePIDCommand extends PIDCommand {

  public ReplaceMePIDCommand() {
    super(new PIDController(0, 0, 0),
          // This should return the measurement
          () -> 0,
          // This is the setpoint (can also be a constant)
          () -> 0,
          // This uses the output
          (output) -> {});
    // Use addRequirements() here to declare subsystem dependencies.
    // Configure additional PID options by calling `getController` here.
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }
}
