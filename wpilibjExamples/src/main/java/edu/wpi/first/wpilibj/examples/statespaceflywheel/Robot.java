package edu.wpi.first.wpilibj.examples.statespaceflywheel;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.examples.statespaceflywheel.subsystems.Flywheel;

public class Robot extends TimedRobot {

  public static Flywheel m_flywheel;

  @Override
  public void robotInit() {
    m_flywheel = new Flywheel();
  }
}
