/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

public class DIOCrossConnectFixture implements ITestFixture {

  private static final Logger logger = Logger.getLogger(DIOCrossConnectFixture.class.getName());

  private final DigitalInput input;
  private final DigitalOutput output;
  private boolean m_allocated;

  public DIOCrossConnectFixture(DigitalInput input, DigitalOutput output) {
    assert input != null;
    assert output != null;
    this.input = input;
    this.output = output;
    m_allocated = false;
  }

  public DIOCrossConnectFixture(Integer input, Integer output) {
    assert input != null;
    assert output != null;
    assert !input.equals(output);
    this.input = new DigitalInput(input);
    this.output = new DigitalOutput(output);
    m_allocated = true;
  }

  public DigitalInput getInput() {
    return input;
  }

  public DigitalOutput getOutput() {
    return output;
  }

  @Override
  public boolean setup() {
    return true;
  }

  @Override
  public boolean reset() {
    try {
      input.cancelInterrupts();
    } catch (IllegalStateException e) {
      // This will happen if the interrupt has not been allocated for this test.
    }
    output.set(false);
    return true;
  }

  @Override
  public boolean teardown() {
    logger.log(Level.FINE, "Begining teardown");
    if (m_allocated) {
      input.free();
      output.free();
      m_allocated = false;
    }
    return true;
  }

}
