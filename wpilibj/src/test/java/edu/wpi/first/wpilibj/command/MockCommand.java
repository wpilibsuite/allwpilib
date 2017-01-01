/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * A class to simulate a simple command The command keeps track of how many times each method was
 * called.
 */
public class MockCommand extends Command {
  private int m_initializeCount = 0;
  private int m_executeCount = 0;
  private int m_isFinishedCount = 0;
  private boolean m_hasFinished = false;
  private int m_endCount = 0;
  private int m_interruptedCount = 0;

  protected void initialize() {
    ++m_initializeCount;
  }

  protected void execute() {
    ++m_executeCount;
  }

  protected boolean isFinished() {
    ++m_isFinishedCount;
    return isHasFinished();
  }

  protected void end() {
    ++m_endCount;
  }

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
   * @return what value the isFinished method will return.
   */
  public boolean isHasFinished() {
    return m_hasFinished;
  }

  /**
   * @param hasFinished set what value the isFinished method will return.
   */
  public void setHasFinished(boolean hasFinished) {
    m_hasFinished = hasFinished;
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

}
