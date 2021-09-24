// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Relay;

/** A connection between a {@link Relay} and two {@link DigitalInput DigitalInputs}. */
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
  public void setup() {
    initialize();
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
    if (!m_freed) {
      m_relay.close();
      m_inputOne.close();
      m_inputTwo.close();
      m_freed = true;
    } else {
      throw new RuntimeException(
          "You attempted to free the "
              + RelayCrossConnectFixture.class.getSimpleName()
              + " multiple times");
    }
  }
}
