// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.fixtures.FakeEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

/** Test to see if the FPGA properly recognizes a mock Encoder input. */
public class EncoderTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(EncoderTest.class.getName());
  private static FakeEncoderFixture encoder = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Test data generator. This method is called the the JUnit parametrized test runner and returns a
   * Collection of Arrays. For each Array in the Collection, each array element corresponds to a
   * parameter in the constructor.
   */
  public static Collection<Integer[]> generateData() {
    return TestBench.getEncoderDIOCrossConnectCollection();
  }

  @AfterAll
  public static void tearDownAfterAll() {
    encoder.teardown();
    encoder = null;
  }

  /** Sets up the test and verifies that the test was reset to the default state. */
  @BeforeEach
  public void setUp() {
    encoder.setup();
  }

  @AfterEach
  public void tearDown() {
    encoder.reset();
  }

  /**
   * Tests to see if Encoders initialize to zero.
   *
   * @param inputA The port number for inputA
   * @param outputA The port number for outputA
   * @param inputB The port number for inputB
   * @param outputB The port number for outputB
   * @param flip whether or not these set of values require the encoder to be reversed (0 or 1)
   */
  @ParameterizedTest
  @MethodSource("generateData")
  public void testDefaultState(int inputA, int outputA, int inputB, int outputB, boolean flip) {
    init(inputA, outputA, inputB, outputB, flip);
    int value = encoder.getEncoder().get();
    assertTrue(value == 0, errorMessage(0, value, inputA, outputA, inputB, outputB));
  }

  /**
   * Tests to see if Encoders can count up successfully.
   *
   * @param inputA The port number for inputA
   * @param outputA The port number for outputA
   * @param inputB The port number for inputB
   * @param outputB The port number for outputB
   * @param flip whether or not these set of values require the encoder to be reversed (0 or 1)
   */
  @ParameterizedTest
  @MethodSource("generateData")
  public void testCountUp(int inputA, int outputA, int inputB, int outputB, boolean flip) {
    init(inputA, outputA, inputB, outputB, flip);
    int goal = 100;
    encoder.getFakeEncoderSource().setCount(goal);
    encoder.getFakeEncoderSource().setForward(flip);
    encoder.getFakeEncoderSource().execute();
    int value = encoder.getEncoder().get();
    assertTrue(value == goal, errorMessage(goal, value, inputA, outputA, inputB, outputB));
  }

  /**
   * Tests to see if Encoders can count down successfully.
   *
   * @param inputA The port number for inputA
   * @param outputA The port number for outputA
   * @param inputB The port number for inputB
   * @param outputB The port number for outputB
   * @param flip whether or not these set of values require the encoder to be reversed (0 or 1)
   */
  @ParameterizedTest
  @MethodSource("generateData")
  public void testCountDown(int inputA, int outputA, int inputB, int outputB, boolean flip) {
    init(inputA, outputA, inputB, outputB, flip);
    int goal = -100;
    encoder.getFakeEncoderSource().setCount(goal); // Goal has to be positive
    encoder.getFakeEncoderSource().setForward(!flip);
    encoder.getFakeEncoderSource().execute();
    int value = encoder.getEncoder().get();
    assertTrue(value == goal, errorMessage(goal, value, inputA, outputA, inputB, outputB));
  }

  /**
   * Constructs a parametrized Encoder Test.
   *
   * @param inputA The port number for inputA
   * @param outputA The port number for outputA
   * @param inputB The port number for inputB
   * @param outputB The port number for outputB
   * @param flip whether or not these set of values require the encoder to be reversed (0 or 1)
   */
  private void init(int inputA, int outputA, int inputB, int outputB, boolean flip) {
    // If the encoder from a previous test is allocated then we must free its
    // members
    if (encoder != null) {
      encoder.teardown();
    }
    encoder = new FakeEncoderFixture(inputA, outputA, inputB, outputB);
  }

  /**
   * Creates a simple message with the error that was encountered for the Encoders.
   *
   * @param goal The goal that was trying to be reached
   * @param trueValue The actual value that was reached by the test
   * @param inputA The port number for inputA
   * @param outputA The port number for outputA
   * @param inputB The port number for inputB
   * @param outputB The port number for outputB
   * @return A fully constructed message with data about where and why the the test failed.
   */
  private String errorMessage(
      int goal, int trueValue, int inputA, int outputA, int inputB, int outputB) {
    return "Encoder ({In,Out}): {"
        + inputA
        + ", "
        + outputA
        + "},{"
        + inputB
        + ", "
        + outputB
        + "} Returned: "
        + trueValue
        + ", Wanted: "
        + goal;
  }
}
