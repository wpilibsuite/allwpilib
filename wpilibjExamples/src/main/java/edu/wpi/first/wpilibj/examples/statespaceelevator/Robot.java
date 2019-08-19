package edu.wpi.first.wpilibj.examples.statespaceelevator;

import edu.wpi.first.wpilibj.TimedRobot;

import edu.wpi.first.wpilibj.examples.statespaceelevator.subsystems.Elevator;

public class Robot extends TimedRobot {
  public static Elevator m_elevator;

  @Override
  public void robotInit() {
    m_elevator = new Elevator();
  }
}
