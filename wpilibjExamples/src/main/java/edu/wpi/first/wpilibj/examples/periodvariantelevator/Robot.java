package edu.wpi.first.wpilibj.examples.periodvariantelevator;

import edu.wpi.first.wpilibj.TimedRobot;

import edu.wpi.first.wpilibj.examples.periodvariantelevator.subsystems.Elevator;

public class Robot extends TimedRobot {
  public static Elevator m_elevator;

  @Override
  public void robotInit() {
    m_elevator = new Elevator();
  }
}
