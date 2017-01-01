/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

/**
 * Connects a digital input to a digital output.
 */
public class DIOCrossConnectFixture implements ITestFixture {

  private static final Logger logger = Logger.getLogger(DIOCrossConnectFixture.class.getName());

  private final DigitalInput m_input;
  private final DigitalOutput m_output;
  private boolean m_allocated;

  /**
   * Constructs using two pre-allocated digital objects.
   *
   * @param input  The input
   * @param output The output.
   */
  public DIOCrossConnectFixture(DigitalInput input, DigitalOutput output) {
    assert input != null;
    assert output != null;
    m_input = input;
    m_output = output;
    m_allocated = false;
  }

  /**
   * Constructs a {@link DIOCrossConnectFixture} using the ports of the digital objects.
   *
   * @param input  The port of the {@link DigitalInput}
   * @param output The port of the {@link DigitalOutput}
   */
  public DIOCrossConnectFixture(Integer input, Integer output) {
    assert input != null;
    assert output != null;
    assert !input.equals(output);
    m_input = new DigitalInput(input);
    m_output = new DigitalOutput(output);
    m_allocated = true;
  }

  public DigitalInput getInput() {
    return m_input;
  }

  public DigitalOutput getOutput() {
    return m_output;
  }

  @Override
  public boolean setup() {
    return true;
  }

  @Override
  public boolean reset() {
    try {
      m_input.cancelInterrupts();
    } catch (IllegalStateException ex) {
      // This will happen if the interrupt has not been allocated for this test.
    }
    m_output.set(false);
    return true;
  }

  @Override
  public boolean teardown() {
    logger.log(Level.FINE, "Begining teardown");
    if (m_allocated) {
      m_input.free();
      m_output.free();
      m_allocated = false;
    }
    return true;
  }

}
