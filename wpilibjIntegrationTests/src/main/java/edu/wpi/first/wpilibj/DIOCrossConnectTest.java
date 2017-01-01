/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Tests to see if the Digital ports are working properly.
 */
@RunWith(Parameterized.class)
public class DIOCrossConnectTest extends AbstractInterruptTest {
  private static final Logger logger = Logger.getLogger(DIOCrossConnectTest.class.getName());

  private static DIOCrossConnectFixture dio = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Default constructor for the DIOCrossConnectTest This test is parameterized in order to allow it
   * to be tested using a variety of different input/output pairs without duplicate code.<br> This
   * class takes Integer port values instead of DigitalClasses because it would force them to be
   * instantiated at the same time which could (untested) cause port binding errors.
   *
   * @param input  The port for the input wire
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
  @Parameters(name = "{index}: Input Port: {0} Output Port: {1}")
  public static Collection<Integer[]> generateData() {
    // In this example, the parameter generator returns a List of
    // arrays. Each array has two elements: { Digital input port, Digital output
    // port}.
    // These data are hard-coded into the class, but they could be
    // generated or loaded in any way you like.
    return TestBench.getInstance().getDIOCrossConnectCollection();
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    dio.teardown();
    dio = null;
  }

  @After
  public void tearDown() throws Exception {
    dio.reset();
  }

  /**
   * Tests to see if the DIO can create and recognize a high value.
   */
  @Test
  public void testSetHigh() {
    dio.getOutput().set(true);
    assertTrue("DIO Not High after no delay", dio.getInput().get());
    Timer.delay(.02);
    assertTrue("DIO Not High after .05s delay", dio.getInput().get());
  }

  /**
   * Tests to see if the DIO can create and recognize a low value.
   */
  @Test
  public void testSetLow() {
    dio.getOutput().set(false);
    assertFalse("DIO Not Low after no delay", dio.getInput().get());
    Timer.delay(.02);
    assertFalse("DIO Not Low after .05s delay", dio.getInput().get());
  }

  /**
   * Tests to see if the DIO PWM functionality works.
   */
  @Test
  public void testDIOpulseWidthModulation() {
    dio.getOutput().set(false);
    assertFalse("DIO Not Low after no delay", dio.getInput().get());
    //Set frequency to 2.0 Hz
    dio.getOutput().setPWMRate(2.0);
    //Enable PWM, but leave it off.
    dio.getOutput().enablePWM(0.0);
    Timer.delay(0.5);
    dio.getOutput().updateDutyCycle(0.5);
    dio.getInput().requestInterrupts();
    dio.getInput().setUpSourceEdge(false, true);
    //TODO: Add return value from WaitForInterrupt
    dio.getInput().waitForInterrupt(3.0, true);
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

    assertFalse("DIO Not Low after first delay", firstCycle);
    assertTrue("DIO Not High after second delay", secondCycle);
    assertFalse("DIO Not Low after third delay", thirdCycle);
    assertTrue("DIO Not High after forth delay", forthCycle);
    assertFalse("DIO Not Low after fifth delay", fifthCycle);
    assertTrue("DIO Not High after sixth delay", sixthCycle);
    assertFalse("DIO Not Low after seventh delay", seventhCycle);
    assertFalse("DIO Not Low after stopping first read", firstAfterStop);
    assertFalse("DIO Not Low after stopping second read", secondAfterStop);
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#giveInterruptableSensorBase()
   */
  @Override
  InterruptableSensorBase giveInterruptableSensorBase() {
    return dio.getInput();
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#freeInterruptableSensorBase()
   */
  @Override
  void freeInterruptableSensorBase() {
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
