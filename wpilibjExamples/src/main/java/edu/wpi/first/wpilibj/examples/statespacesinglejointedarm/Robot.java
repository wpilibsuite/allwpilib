package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm;

import edu.wpi.first.wpilibj.TimedRobot;

import edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems.SingleJointedArm;

public class Robot extends TimedRobot {
  public static SingleJointedArm m_singleJointedArm;

  @Override
  public void robotInit() {
    m_singleJointedArm = new SingleJointedArm();
  }
}
