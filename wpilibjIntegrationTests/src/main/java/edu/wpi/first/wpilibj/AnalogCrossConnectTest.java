/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;
import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Test that covers the {@link AnalogCrossConnectFixture}.
 */
public class AnalogCrossConnectTest extends AbstractInterruptTest {
  private static final Logger logger = Logger.getLogger(AnalogCrossConnectTest.class.getName());

  private static AnalogCrossConnectFixture analogIO;

  static final double kDelayTime = 0.01;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }


  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
    analogIO = TestBench.getAnalogCrossConnectFixture();
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    analogIO.teardown();
    analogIO = null;
  }

  @Before
  public void setUp() throws Exception {
    analogIO.setup();
  }


  @Test
  public void testAnalogOuput() {
    for (int i = 0; i < 50; i++) {
      analogIO.getOutput().setVoltage(i / 10.0);
      Timer.delay(kDelayTime);
      assertEquals(analogIO.getOutput().getVoltage(), analogIO.getInput().getVoltage(), 0.01);
    }
  }

  @Test
  public void testAnalogTriggerBelowWindow() {
    // Given
    AnalogTrigger trigger = new AnalogTrigger(analogIO.getInput());
    trigger.setLimitsVoltage(2.0, 3.0);

    // When the output voltage is than less the lower limit
    analogIO.getOutput().setVoltage(1.0);
    Timer.delay(kDelayTime);

    // Then the analog trigger is not in the window and the trigger state is off
    assertFalse("Analog trigger is in the window (2V, 3V)", trigger.getInWindow());
    assertFalse("Analog trigger is on", trigger.getTriggerState());

    trigger.free();
  }

  @Test
  public void testAnalogTriggerInWindow() {
    // Given
    AnalogTrigger trigger = new AnalogTrigger(analogIO.getInput());
    trigger.setLimitsVoltage(2.0, 3.0);

    // When the output voltage is within the lower and upper limits
    analogIO.getOutput().setVoltage(2.5f);
    Timer.delay(kDelayTime);

    // Then the analog trigger is in the window and the trigger state is off
    assertTrue("Analog trigger is not in the window (2V, 3V)", trigger.getInWindow());
    assertFalse("Analog trigger is on", trigger.getTriggerState());

    trigger.free();
  }

  @Test
  public void testAnalogTriggerAboveWindow() {
    // Given
    AnalogTrigger trigger = new AnalogTrigger(analogIO.getInput());
    trigger.setLimitsVoltage(2.0, 3.0);

    // When the output voltage is greater than the upper limit
    analogIO.getOutput().setVoltage(4.0);
    Timer.delay(kDelayTime);

    // Then the analog trigger is not in the window and the trigger state is on
    assertFalse("Analog trigger is in the window (2V, 3V)", trigger.getInWindow());
    assertTrue("Analog trigger is not on", trigger.getTriggerState());

    trigger.free();
  }

  @Test
  public void testAnalogTriggerCounter() {
    // Given
    AnalogTrigger trigger = new AnalogTrigger(analogIO.getInput());
    trigger.setLimitsVoltage(2.0, 3.0);
    Counter counter = new Counter(trigger);

    // When the analog output is turned low and high 50 times
    for (int i = 0; i < 50; i++) {
      analogIO.getOutput().setVoltage(1.0);
      Timer.delay(kDelayTime);
      analogIO.getOutput().setVoltage(4.0);
      Timer.delay(kDelayTime);
    }

    // Then the counter should be at 50
    assertEquals("Analog trigger counter did not count 50 ticks", 50, counter.get());
  }

  @Test(expected = RuntimeException.class)
  public void testRuntimeExceptionOnInvalidAccumulatorPort() {
    analogIO.getInput().getAccumulatorCount();
  }

  private AnalogTrigger m_interruptTrigger;
  private AnalogTriggerOutput m_interruptTriggerOutput;

  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#giveInterruptableSensorBase()
   */
  @Override
  InterruptableSensorBase giveInterruptableSensorBase() {
    m_interruptTrigger = new AnalogTrigger(analogIO.getInput());
    m_interruptTrigger.setLimitsVoltage(2.0, 3.0);
    m_interruptTriggerOutput = new AnalogTriggerOutput(m_interruptTrigger,
        AnalogTriggerType.kState);
    return m_interruptTriggerOutput;
  }


  /*
   * (non-Javadoc)
   *
   * @see
   * edu.wpi.first.wpilibj.AbstractInterruptTest#freeInterruptableSensorBase()
   */
  @Override
  void freeInterruptableSensorBase() {
    m_interruptTriggerOutput.cancelInterrupts();
    m_interruptTriggerOutput.free();
    m_interruptTriggerOutput = null;
    m_interruptTrigger.free();
    m_interruptTrigger = null;
  }


  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.AbstractInterruptTest#setInterruptHigh()
   */
  @Override
  void setInterruptHigh() {
    analogIO.getOutput().setVoltage(4.0);
    Timer.delay(kDelayTime);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.AbstractInterruptTest#setInterruptLow()
   */
  @Override
  void setInterruptLow() {
    analogIO.getOutput().setVoltage(1.0);
    Timer.delay(kDelayTime);
  }
}
