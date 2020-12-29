// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.romireference.commands;

import java.util.function.Supplier;
import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj.examples.romireference.subsystems.Drivetrain;

public class TeleopArcadeDrive extends CommandBase {
  private final Drivetrain m_drivetrain;
  private final Supplier<Double> m_xaxisSpeedSupplier;
  private final Supplier<Double> m_zaxisRotateSupplier;

  /**
   * Creates a new TeleopArcadeDrive.
   *
   * @param drive   The drivetrain subsystem on which this command will run
   * @param xSpeedSupplier Lambda supplier of forward/backward speed
   * @param zRotateSupplie Lambda supplier of rotational speed
   */
  public TeleopArcadeDrive(Drivetrain drivetrain, Supplier<Double> xaxisSpeedSupplier,
      Supplier<Double> zaxisRotateSuppplier) {
    m_drivetrain = drivetrain;
    m_xaxisSpeedSupplier = xaxisSpeedSupplier;
    m_zaxisRotateSupplier = zaxisRotateSuppplier;
    addRequirements(drivetrain);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    m_drivetrain.arcadeDrive(m_xaxisSpeedSupplier.get(), m_zaxisRotateSupplier.get());
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }

}
