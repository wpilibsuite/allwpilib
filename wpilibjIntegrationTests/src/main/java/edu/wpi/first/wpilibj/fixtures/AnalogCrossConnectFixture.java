/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * A fixture that connects an {@link AnalogInput} and an {@link AnalogOutput}.
 */
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
  public boolean setup() {
    initialize();
    m_output.setVoltage(0);
    return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public boolean reset() {
    initialize();
    return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public boolean teardown() {
    m_input.free();
    m_output.free();
    return true;
  }

  /**
   * Analog Output.
   */
  public final AnalogOutput getOutput() {
    initialize();
    return m_output;
  }

  public final AnalogInput getInput() {
    initialize();
    return m_input;
  }


}
