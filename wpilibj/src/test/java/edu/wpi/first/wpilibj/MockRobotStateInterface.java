/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * A concrete implementation of the robot state interface that can be used in UnitTests.
 */
public class MockRobotStateInterface implements RobotState.Interface {
  @Override
  public boolean isDisabled() {
    return false;
  }

  @Override
  public boolean isEnabled() {
    return true;
  }

  @Override
  public boolean isOperatorControl() {
    return false;
  }

  @Override
  public boolean isAutonomous() {
    return false;
  }

  @Override
  public boolean isTest() {
    return true;
  }
}
