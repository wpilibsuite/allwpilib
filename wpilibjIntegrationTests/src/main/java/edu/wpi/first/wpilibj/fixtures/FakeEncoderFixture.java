/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.mockhardware.FakeEncoderSource;

/**
 * An encoder that uses two {@link DIOCrossConnectFixture DIOCrossConnectFixtures} to test the
 * {@link Encoder}.
 */
public class FakeEncoderFixture implements ITestFixture {
  private static final Logger logger = Logger.getLogger(FakeEncoderFixture.class.getName());

  private final DIOCrossConnectFixture m_dio1;
  private final DIOCrossConnectFixture m_dio2;
  private boolean m_allocated;

  private final FakeEncoderSource m_source;
  private int[] m_sourcePort = new int[2];
  private final Encoder m_encoder;
  private int[] m_encoderPort = new int[2];

  /**
   * Constructs a FakeEncoderFixture from two DIOCrossConnectFixture.
   */
  public FakeEncoderFixture(DIOCrossConnectFixture dio1, DIOCrossConnectFixture dio2) {
    assert dio1 != null;
    assert dio2 != null;
    m_dio1 = dio1;
    m_dio2 = dio2;
    m_allocated = false;
    m_source = new FakeEncoderSource(dio1.getOutput(), dio2.getOutput());
    m_encoder = new Encoder(dio1.getInput(), dio2.getInput());
  }

  /**
   * Construcst a FakeEncoderFixture from a set of Digital I/O ports.
   */
  public FakeEncoderFixture(int inputA, int outputA, int inputB, int outputB) {
    assert outputA != outputB;
    assert outputA != inputA;
    assert outputA != inputB;
    assert outputB != inputA;
    assert outputB != inputB;
    assert inputA != inputB;
    m_dio1 = new DIOCrossConnectFixture(inputA, outputA);
    m_dio2 = new DIOCrossConnectFixture(inputB, outputB);
    m_allocated = true;
    m_sourcePort[0] = outputA;
    m_sourcePort[1] = outputB;
    m_encoderPort[0] = inputA;
    m_encoderPort[1] = inputB;
    m_source = new FakeEncoderSource(m_dio1.getOutput(), m_dio2.getOutput());
    m_encoder = new Encoder(m_dio1.getInput(), m_dio2.getInput());
  }

  public FakeEncoderSource getFakeEncoderSource() {
    return m_source;
  }

  public Encoder getEncoder() {
    return m_encoder;
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
    m_dio1.reset();
    m_dio2.reset();
    m_encoder.reset();
    return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
   */
  @Override
  public boolean teardown() {
    logger.fine("Begining teardown");
    m_source.free();
    logger.finer("Source freed " + m_sourcePort[0] + ",  " + m_sourcePort[1]);
    m_encoder.free();
    logger.finer("Encoder freed " + m_encoderPort[0] + ", " + m_encoderPort[1]);
    if (m_allocated) {
      m_dio1.teardown();
      m_dio2.teardown();
    }
    return true;
  }


}
