package edu.wpi.first.wpilibj.examples.periodvariantsinglejointedarm;

import edu.wpi.first.wpilibj.TimedRobot;

import edu.wpi.first.wpilibj.examples.periodvariantsinglejointedarm.subsystems.SingleJointedArm;

public class Robot extends TimedRobot {
  public static SingleJointedArm m_singleJointedArm;

  @Override
  public void robotInit() {
    m_singleJointedArm = new SingleJointedArm();
  }
}
