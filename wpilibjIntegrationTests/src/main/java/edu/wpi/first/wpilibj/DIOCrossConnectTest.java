// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

/** Tests to see if the Digital ports are working properly. */
public class DIOCrossConnectTest extends AbstractInterruptTest {
  private static final Logger logger = Logger.getLogger(DIOCrossConnectTest.class.getName());

  private static DIOCrossConnectFixture dio = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Default constructor for the DIOCrossConnectTest This test is parameterized in order to allow it
   * to be tested using a variety of different input/output pairs without duplicate code.<br>
   * This class takes Integer port values instead of DigitalClasses because it would force them to
   * be instantiated at the same time which could (untested) cause port binding errors.
   *
   * @param input The port for the input wire
   * @param output The port for the output wire
   */
  public DIOCrossConnectTest(Integer input, Integer output) {
    if (dio != null) {
      dio.teardown();
    }
    dio = new DIOCrossConnectFixture(input, output);
  }

  /**
   * Test data generator. This method is called the the JUnit parameterized test runner and returns
   * a Collection of Arrays. For each Array in the Collection, each array element corresponds to a
   * parameter in the constructor.
   */
  public static Collection<Integer[]> generateData() {
    // In this example, the parameter generator returns a List of
    // arrays. Each array has two elements: { Digital input port, Digital output
    // port}.
    // These data are hard-coded into the class, but they could be
    // generated or loaded in any way you like.
    return TestBench.getDIOCrossConnectCollection();
  }

  @AfterAll
  public static void tearDownAfterAll() {
    dio.teardown();
    dio = null;
  }

  @AfterEach
  public void tearDown() {
    dio.reset();
  }

  /** Tests to see if the DIO can create and recognize a high value. */
  @ParameterizedTest(name = "{index}: Input Port: {0} Output Port: {1}")
  @MethodSource("generateData")
  public void testSetHigh(int input, int output) {
    if (dio != null) {
      dio.teardown();
    }
    dio = new DIOCrossConnectFixture(input, output);

    dio.getOutput().set(true);
    assertTrue(dio.getInput().get(), "DIO Not High after no delay");
    Timer.delay(0.02);
    assertTrue(dio.getInput().get(), "DIO Not High after .05s delay");
  }

  /** Tests to see if the DIO can create and recognize a low value. */
  @ParameterizedTest(name = "{index}: Input Port: {0} Output Port: {1}")
  @MethodSource("generateData")
  public void testSetLow(int input, int output) {
    if (dio != null) {
      dio.teardown();
    }
    dio = new DIOCrossConnectFixture(input, output);

    dio.getOutput().set(false);
    assertFalse(dio.getInput().get(), "DIO Not Low after no delay");
    Timer.delay(0.02);
    assertFalse(dio.getInput().get(), "DIO Not Low after .05s delay");
  }

  /** Tests to see if the DIO PWM functionality works. */
  @ParameterizedTest(name = "{index}: Input Port: {0} Output Port: {1}")
  @MethodSource("generateData")
  public void testDIOpulseWidthModulation(int input, int output) {
    if (dio != null) {
      dio.teardown();
    }
    dio = new DIOCrossConnectFixture(input, output);

    dio.getOutput().set(false);
    assertFalse(dio.getInput().get(), "DIO Not Low after no delay");
    // Set frequency to 2.0 Hz
    dio.getOutput().setPWMRate(2.0);
    // Enable PWM, but leave it off.
    dio.getOutput().enablePWM(0.0);
    Timer.delay(0.5);
    dio.getOutput().updateDutyCycle(0.5);
    try (var interruptHandler = new SynchronousInterrupt(dio.getInput())) {
      interruptHandler.setInterruptEdges(false, true);
      // TODO: Add return value from WaitForInterrupt
      interruptHandler.waitForInterrupt(3.0, true);
    }
    Timer.delay(0.5);
    final boolean firstCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean secondCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean thirdCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean forthCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean fifthCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean sixthCycle = dio.getInput().get();
    Timer.delay(0.5);
    final boolean seventhCycle = dio.getInput().get();
    dio.getOutput().disablePWM();
    Timer.delay(0.5);
    final boolean firstAfterStop = dio.getInput().get();
    Timer.delay(0.5);
    final boolean secondAfterStop = dio.getInput().get();

    assertFalse(firstCycle, "DIO Not Low after first delay");
    assertTrue(secondCycle, "DIO Not High after second delay");
    assertFalse(thirdCycle, "DIO Not Low after third delay");
    assertTrue(forthCycle, "DIO Not High after forth delay");
    assertFalse(fifthCycle, "DIO Not Low after fifth delay");
    assertTrue(sixthCycle, "DIO Not High after sixth delay");
    assertFalse(seventhCycle, "DIO Not Low after seventh delay");
    assertFalse(firstAfterStop, "DIO Not Low after stopping first read");
    assertFalse(secondAfterStop, "DIO Not Low after stopping second read");
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#giveSource()
   */
  @Override
  DigitalSource giveSource() {
    return dio.getInput();
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#freeSource()
   */
  @Override
  void freeSource() {
    // Handled in the fixture
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.AbstractInterruptTest#setInterruptHigh()
   */
  @Override
  void setInterruptHigh() {
    dio.getOutput().set(true);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.AbstractInterruptTest#setInterruptLow()
   */
  @Override
  void setInterruptLow() {
    dio.getOutput().set(false);
  }
}
