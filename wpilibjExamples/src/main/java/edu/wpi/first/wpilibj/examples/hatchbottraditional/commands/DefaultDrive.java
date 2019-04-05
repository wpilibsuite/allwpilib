package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.experimental.command.SendableCommandBase;

/**
 * A command to drive the robot with joystick input (passed in as {@link DoubleSupplier}s).
 * Written explicitly for pedagogical purposes - actual code should inline a command this simple
 * with {@link edu.wpi.first.wpilibj.experimental.command.RunCommand}.
 */
public class DefaultDrive extends SendableCommandBase {

  private final DriveSubsystem m_drive;
  private final DoubleSupplier m_forward;
  private final DoubleSupplier m_rotation;

  public DefaultDrive(DriveSubsystem subsystem, DoubleSupplier forward, DoubleSupplier rotation){
    m_drive = subsystem;
    m_forward = forward;
    m_rotation = rotation;
    addRequirements(m_drive);
  }

  @Override
  public void execute() {
    m_drive.arcadeDrive(m_forward.getAsDouble(), m_rotation.getAsDouble());
  }
}
