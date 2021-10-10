// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.mockhardware.FakeCounterSource;
import java.util.logging.Level;
import java.util.logging.Logger;

/** A fixture that can test the {@link Counter} using a {@link DIOCrossConnectFixture}. */
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
   * @param input the input port
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
  public void setup() {}

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public void reset() {
    m_counter.reset();
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public void teardown() {
    logger.log(Level.FINE, "Beginning teardown");
    m_counter.close();
    m_source.close();
    if (m_allocated) { // Only tear down the dio if this class allocated it
      m_dio.teardown();
      m_allocated = false;
    }
  }
}
