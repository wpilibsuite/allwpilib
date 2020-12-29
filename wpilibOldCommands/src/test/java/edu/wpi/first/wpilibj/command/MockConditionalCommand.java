// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

public class MockConditionalCommand extends ConditionalCommand {
  private boolean m_condition;
  private int m_initializeCount;
  private int m_executeCount;
  private int m_isFinishedCount;
  private int m_endCount;
  private int m_interruptedCount;

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

  @Override
  protected void initialize() {
    ++m_initializeCount;
  }

  @Override
  protected void execute() {
    ++m_executeCount;
  }

  @Override
  protected boolean isFinished() {
    ++m_isFinishedCount;
    return super.isFinished();
  }

  @Override
  protected void end() {
    ++m_endCount;
  }

  @Override
  protected void interrupted() {
    ++m_interruptedCount;
  }


  /**
   * How many times the initialize method has been called.
   */
  public int getInitializeCount() {
    return m_initializeCount;
  }

  /**
   * If the initialize method has been called at least once.
   */
  public boolean hasInitialized() {
    return getInitializeCount() > 0;
  }

  /**
   * How many time the execute method has been called.
   */
  public int getExecuteCount() {
    return m_executeCount;
  }

  /**
   * How many times the isFinished method has been called.
   */
  public int getIsFinishedCount() {
    return m_isFinishedCount;
  }

  /**
   * How many times the end method has been called.
   */
  public int getEndCount() {
    return m_endCount;
  }

  /**
   * If the end method has been called at least once.
   */
  public boolean hasEnd() {
    return getEndCount() > 0;
  }

  /**
   * How many times the interrupted method has been called.
   */
  public int getInterruptedCount() {
    return m_interruptedCount;
  }

  /**
   * If the interrupted method has been called at least once.
   */
  public boolean hasInterrupted() {
    return getInterruptedCount() > 0;
  }

  /**
   * Reset internal counters.
   */
  public void resetCounters() {
    m_condition = false;
    m_initializeCount = 0;
    m_executeCount = 0;
    m_isFinishedCount = 0;
    m_endCount = 0;
    m_interruptedCount = 0;
  }
}
