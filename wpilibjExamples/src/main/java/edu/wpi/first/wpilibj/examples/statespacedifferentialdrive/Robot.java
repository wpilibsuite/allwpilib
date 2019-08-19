package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems.Drivetrain;

public class Robot extends TimedRobot {
  public static Drivetrain m_drivetrain;

  @Override
  public void robotInit() {
    m_drivetrain = new Drivetrain();
  }
}
