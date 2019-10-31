package edu.wpi.first.wpilibj.commands.wpilibj2.profiledpidsubsystem;

import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj2.command.ProfiledPIDSubsystem;

public class ReplaceMeProfiledPIDSubsystem extends ProfiledPIDSubsystem {

  public ReplaceMeProfiledPIDSubsystem() {
    super(new ProfiledPIDController(0, 0, 0,
                                    new TrapezoidProfile.Constraints(0, 0)));
  }

  @Override
  public void useOutput(double output, TrapezoidProfile.State setpoint) {
    // Use the output (and optionally the setpoint) here
  }

  @Override
  public TrapezoidProfile.State getGoal() {
    // Return the goal state here
    return new TrapezoidProfile.State();
  }

  @Override
  public double getMeasurement() {
    // Return the process variable measurement here
    return 0;
  }
}
