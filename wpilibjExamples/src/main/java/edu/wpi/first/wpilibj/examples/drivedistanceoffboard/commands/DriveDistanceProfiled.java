package edu.wpi.first.wpilibj.examples.drivedistanceoffboard.commands;

import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj2.command.TrapezoidProfileCommand;

import edu.wpi.first.wpilibj.examples.drivedistanceoffboard.subsystems.DriveSubsystem;

import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kMaxAccelerationMetersPerSecondSquared;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kMaxSpeedMetersPerSecond;

/**
 * Drives a set distance using a motion profile.
 */
public class DriveDistanceProfiled extends TrapezoidProfileCommand {

  /**
   * Creates a new DriveDistanceProfiled command.
   *
   * @param meters The distance to drive.
   * @param drive The drive subsystem to use.
   */
  public DriveDistanceProfiled(double meters, DriveSubsystem drive) {
    super(
        new TrapezoidProfile(
            // Limit the max acceleration and velocity
            new TrapezoidProfile.Constraints(kMaxSpeedMetersPerSecond,
                                             kMaxAccelerationMetersPerSecondSquared),
            // End at desired position in meters; implicitly starts at 0
            new TrapezoidProfile.State(meters, 0)),
        // Pipe the profile state to the drive
        setpointState -> drive.setDriveStates(setpointState, setpointState),
        // Require the drive
        drive);
    // Reset drive encoders since we're starting at 0
    drive.resetEncoders();
  }
}
