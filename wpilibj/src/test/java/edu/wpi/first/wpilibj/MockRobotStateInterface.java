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
