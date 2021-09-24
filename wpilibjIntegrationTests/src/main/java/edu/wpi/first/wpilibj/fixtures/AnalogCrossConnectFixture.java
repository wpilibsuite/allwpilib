// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;

/** A fixture that connects an {@link AnalogInput} and an {@link AnalogOutput}. */
public abstract class AnalogCrossConnectFixture implements ITestFixture {
  private boolean m_initialized = false;

  private AnalogInput m_input;
  private AnalogOutput m_output;

  protected abstract AnalogInput giveAnalogInput();

  protected abstract AnalogOutput giveAnalogOutput();

  private void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_input = giveAnalogInput();
        m_output = giveAnalogOutput();
        m_initialized = true;
      }
    }
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
   */
  @Override
  public void setup() {
    initialize();
    m_output.setVoltage(0);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public void reset() {
    initialize();
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public void teardown() {
    m_input.close();
    m_output.close();
  }

  /** Analog Output. */
  public final AnalogOutput getOutput() {
    initialize();
    return m_output;
  }

  public final AnalogInput getInput() {
    initialize();
    return m_input;
  }
}
