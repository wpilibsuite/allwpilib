/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.mockhardware.FakeCounterSource;

/**
 * A fixture that can test the {@link Counter} using a {@link DIOCrossConnectFixture}.
 */
public class FakeCounterFixture implements ITestFixture {
  private static final Logger logger = Logger.getLogger(FakeEncoderFixture.class.getName());

  private final DIOCrossConnectFixture m_dio;
  private boolean m_allocated;
  private final FakeCounterSource m_source;
  private final Counter m_counter;

  /**
   * Constructs a FakeCounterFixture.
   *
   * @param dio A previously allocated DIOCrossConnectFixture (must be freed outside this class)
   */
  public FakeCounterFixture(DIOCrossConnectFixture dio) {
    m_dio = dio;
    m_allocated = false;
    m_source = new FakeCounterSource(dio.getOutput());
    m_counter = new Counter(dio.getInput());
  }


  /**
   * Constructs a FakeCounterFixture using two port numbers.
   *
   * @param input  the input port
   * @param output the output port
   */
  public FakeCounterFixture(int input, int output) {
    m_dio = new DIOCrossConnectFixture(input, output);
    m_allocated = true;
    m_source = new FakeCounterSource(m_dio.getOutput());
    m_counter = new Counter(m_dio.getInput());
  }

  /**
   * Retrieves the FakeCouterSource for use.
   *
   * @return the FakeCounterSource
   */
  public FakeCounterSource getFakeCounterSource() {
    return m_source;
  }

  /**
   * Gets the Counter for use.
   *
   * @return the Counter
   */
  public Counter getCounter() {
    return m_counter;
  }


  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
   */
  @Override
  public boolean setup() {
    return true;

  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public boolean reset() {
    m_counter.reset();
    return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public boolean teardown() {
    logger.log(Level.FINE, "Begining teardown");
    m_counter.free();
    m_source.free();
    if (m_allocated) { // Only tear down the dio if this class allocated it
      m_dio.teardown();
      m_allocated = false;
    }
    return true;
  }


}
