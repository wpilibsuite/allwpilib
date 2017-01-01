/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Relay;

/**
 * A connection between a {@link Relay} and two {@link DigitalInput DigitalInputs}.
 */
public abstract class RelayCrossConnectFixture implements ITestFixture {

  private DigitalInput m_inputOne;
  private DigitalInput m_inputTwo;
  private Relay m_relay;

  private boolean m_initialized = false;
  private boolean m_freed = false;


  protected abstract Relay giveRelay();

  protected abstract DigitalInput giveInputOne();

  protected abstract DigitalInput giveInputTwo();

  private void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_relay = giveRelay();
        m_inputOne = giveInputOne();
        m_inputTwo = giveInputTwo();
        m_initialized = true;
      }
    }
  }

  public Relay getRelay() {
    initialize();
    return m_relay;
  }

  public DigitalInput getInputOne() {
    initialize();
    return m_inputOne;
  }

  public DigitalInput getInputTwo() {
    initialize();
    return m_inputTwo;
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
   */
  @Override
  public boolean setup() {
    initialize();
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
    if (!m_freed) {
      m_relay.free();
      m_inputOne.free();
      m_inputTwo.free();
      m_freed = true;
    } else {
      throw new RuntimeException("You attempted to free the "
          + RelayCrossConnectFixture.class.getSimpleName() + " multiple times");
    }
    return true;
  }
}
