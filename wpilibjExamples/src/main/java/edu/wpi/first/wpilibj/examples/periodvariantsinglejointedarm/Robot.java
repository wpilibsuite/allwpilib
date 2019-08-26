/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
