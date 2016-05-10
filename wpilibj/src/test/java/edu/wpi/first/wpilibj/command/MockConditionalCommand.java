package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.command.ConditionalCommand;

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
