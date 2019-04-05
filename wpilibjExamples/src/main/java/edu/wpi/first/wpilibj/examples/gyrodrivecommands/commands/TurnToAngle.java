package edu.wpi.first.wpilibj.examples.gyrodrivecommands.commands;

import edu.wpi.first.wpilibj.examples.gyrodrivecommands.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.experimental.command.SynchronousPIDCommand;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static edu.wpi.first.wpilibj.examples.gyrodrivecommands.Constants.DriveConstants.*;

/**
 * A command that will turn the robot to the specified angle.
 */
public class TurnToAngle extends SynchronousPIDCommand {

  public TurnToAngle(double targetAngleDegrees, DriveSubsystem drive) {
    super(new PIDController(kTurnP, kTurnI, kTurnD),
        // Close loop on heading
        drive::getHeading,
        // Set reference to target
        targetAngleDegrees,
        // Pipe output to turn robot
        (output) -> drive.arcadeDrive(0, output),
        // Require the drive
        drive);

    // Set the input range of the controller to match the gyro output
    getController().setInputRange(-180, 180);
    // Set the controller to be continuous (because it is an angle controller)
    getController().setContinuous();
    // Set the controller tolerance - the delta tolerance ensures the robot is stationary at the
    // setpoint before it is considered as having reached the reference
    getController().setAbsoluteTolerance(kTurnToleranceDeg, kTurnRateToleranceDegPerS);
  }

  @Override
  public boolean isFinished() {
    // End when the controller is at the reference.
    return getController().atReference();
  }
}
