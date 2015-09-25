/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.mockhardware.FakeCounterSource;

/**
 * @author jonathanleitschuh
 *
 */
public class FakeCounterFixture implements ITestFixture {
  private static final Logger logger = Logger.getLogger(FakeEncoderFixture.class.getName());

  private final DIOCrossConnectFixture dio;
  private boolean m_allocated;
  private final FakeCounterSource source;
  private final Counter counter;

  /**
   * Constructs a FakeCounterFixture.
   *$
   * @param dio A previously allocated DIOCrossConnectFixture (must be freed
   *        outside this class)
   */
  public FakeCounterFixture(DIOCrossConnectFixture dio) {
    this.dio = dio;
    m_allocated = false;
    source = new FakeCounterSource(dio.getOutput());
    counter = new Counter(dio.getInput());
  }


  /**
   * Constructs a FakeCounterFixture using two port numbers
   *$
   * @param input the input port
   * @param output the output port
   */
  public FakeCounterFixture(int input, int output) {
    this.dio = new DIOCrossConnectFixture(input, output);
    m_allocated = true;
    source = new FakeCounterSource(dio.getOutput());
    counter = new Counter(dio.getInput());
  }

  /**
   * Retrieves the FakeCouterSource for use
   *$
   * @return the FakeCounterSource
   */
  public FakeCounterSource getFakeCounterSource() {
    return source;
  }

  /**
   * Gets the Counter for use
   *$
   * @return the Counter
   */
  public Counter getCounter() {
    return counter;
  }


  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
   */
  @Override
  public boolean setup() {
    return true;

  }

  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
   */
  @Override
  public boolean reset() {
    counter.reset();
    return true;
  }

  /*
   * (non-Javadoc)
   *$
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public boolean teardown() {
    logger.log(Level.FINE, "Begining teardown");
    counter.free();
    source.free();
    if (m_allocated) { // Only tear down the dio if this class allocated it
      dio.teardown();
      m_allocated = false;
    }
    return true;
  }



}
