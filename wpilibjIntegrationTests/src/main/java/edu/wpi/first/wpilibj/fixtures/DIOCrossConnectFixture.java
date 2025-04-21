// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import java.util.logging.Level;
import java.util.logging.Logger;

/** Connects a digital input to a digital output. */
public class DIOCrossConnectFixture implements ITestFixture {
  private static final Logger logger = Logger.getLogger(DIOCrossConnectFixture.class.getName());

  private final DigitalInput m_input;
  private final DigitalOutput m_output;
  private boolean m_allocated;

  /**
   * Constructs using two pre-allocated digital objects.
   *
   * @param input The input
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
   * @param input The port of the {@link DigitalInput}
   * @param output The port of the {@link DigitalOutput}
   */
  public DIOCrossConnectFixture(int input, int output) {
    assert input != output;
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
  public void setup() {}

  @Override
  public void reset() {
    m_output.set(false);
  }

  @Override
  public void teardown() {
    logger.log(Level.FINE, "Beginning teardown");
    if (m_allocated) {
      m_input.close();
      m_output.close();
      m_allocated = false;
    }
  }
}
