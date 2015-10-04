/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * @author jonathanleitschuh
 *
 */
public abstract class AnalogCrossConnectFixture implements ITestFixture {
  private boolean initialized = false;

  private AnalogInput input;
  private AnalogOutput output;

  abstract protected AnalogInput giveAnalogInput();

  abstract protected AnalogOutput giveAnalogOutput();


  private void initialize() {
    synchronized (this) {
      if (!initialized) {
        input = giveAnalogInput();
        output = giveAnalogOutput();
        initialized = true;
      }
    }
  }

  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
   */
  @Override
  public boolean setup() {
    initialize();
    output.setVoltage(0);
    return true;
  }

  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public boolean reset() {
    initialize();
    return true;
  }

  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public boolean teardown() {
    input.free();
    output.free();
    return true;
  }

  /**
   * @return Analog Output
   */
  final public AnalogOutput getOutput() {
    initialize();
    return output;
  }

  final public AnalogInput getInput() {
    initialize();
    return input;
  }


}
