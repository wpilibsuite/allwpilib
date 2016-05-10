/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

public class MockConditionalCommand extends ConditionalCommand {
  private boolean m_condition = false;

  public MockConditionalCommand(MockCommand onTrue, MockCommand onFalse) {
    super(onTrue, onFalse);
  }

  @Override
  protected boolean condition() {
    return m_condition;
  }

  public void setCondition(boolean condition) {
    this.m_condition = condition;
  }
}
