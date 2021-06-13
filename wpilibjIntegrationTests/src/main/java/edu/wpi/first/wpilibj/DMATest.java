// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.motorcontrol.Jaguar;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

@Ignore
public class DMATest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(DMATest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  private AnalogCrossConnectFixture m_analogIO;
  private DigitalOutput m_manualTrigger;
  private PWMMotorController m_pwm;
  private DMA m_dma;
  private DMASample m_dmaSample;

  @Before
  public void setUp() {
    m_analogIO = TestBench.getAnalogCrossConnectFixture();
    m_manualTrigger = new DigitalOutput(7);
    m_pwm = new Jaguar(14);
    m_dma = new DMA();
    m_dmaSample = new DMASample();

    m_dma.addAnalogInput(m_analogIO.getInput());
    m_dma.setExternalTrigger(m_manualTrigger, false, true);
    m_manualTrigger.set(true);
  }

  @After
  public void tearDown() {
    m_dma.close();
    m_manualTrigger.close();
    m_analogIO.teardown();
    m_pwm.close();
  }

  @Test
  public void testPausingWorks() {
    m_dma.start(1024);
    m_dma.setPause(true);
    m_manualTrigger.set(false);

    var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(5));

    assertEquals(DMASample.DMAReadStatus.kTimeout, timedOut);
  }

  @Test
  public void testRemovingTriggersWorks() {
    m_dma.clearExternalTriggers();
    m_dma.start(1024);
    m_manualTrigger.set(false);

    var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(5));

    assertEquals(DMASample.DMAReadStatus.kTimeout, timedOut);
  }

  @Test
  public void testManualTriggerOnlyHappensOnce() {
    m_dma.start(1024);
    m_manualTrigger.set(false);
    var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(5));
    m_manualTrigger.set(true);

    assertEquals(DMASample.DMAReadStatus.kOk, timedOut);
    assertEquals(0, m_dmaSample.getRemaining());
    timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(5));
    assertEquals(DMASample.DMAReadStatus.kTimeout, timedOut);
  }

  @Test
  public void testAnalogIndividualTriggers() {
    m_dma.start(1024);
    for (double i = 0; i < 5; i += 0.5) {
      m_analogIO.getOutput().setVoltage(i);
      // Need to sleep to ensure value sets
      Timer.delay(AnalogCrossConnectTest.kDelayTime);
      m_manualTrigger.set(false);
      var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(1));
      m_manualTrigger.set(true);

      assertEquals(DMASample.DMAReadStatus.kOk, timedOut);
      assertEquals(0, m_dmaSample.getRemaining());
      assertEquals(
          m_analogIO.getInput().getVoltage(),
          m_dmaSample.getAnalogInputVoltage(m_analogIO.getInput()),
          0.01);
    }
  }

  @Test
  public void testAnalogMultipleTriggers() {
    m_dma.start(1024);
    List<Double> values = new ArrayList<>();
    for (double i = 0; i < 5; i += 0.5) {
      m_analogIO.getOutput().setVoltage(i);
      values.add(i);
      // Need to sleep to ensure value sets
      Timer.delay(AnalogCrossConnectTest.kDelayTime);
      m_manualTrigger.set(false);
      Timer.delay(AnalogCrossConnectTest.kDelayTime);
      m_manualTrigger.set(true);
    }

    for (int i = 0; i < values.size(); i++) {
      var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(1));
      assertEquals(DMASample.DMAReadStatus.kOk, timedOut);
      assertEquals(values.size() - i - 1, m_dmaSample.getRemaining());
      assertEquals(values.get(i), m_dmaSample.getAnalogInputVoltage(m_analogIO.getInput()), 0.01);
    }
  }

  @Test
  public void testTimedTriggers() {
    m_dma.setTimedTrigger(Units.millisecondsToSeconds(10));
    m_dma.start(1024);
    Timer.delay(Units.millisecondsToSeconds(100));
    m_dma.setPause(true);

    var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(1));
    assertEquals(DMASample.DMAReadStatus.kOk, timedOut);
    assertTrue("Received more then 5 samples in 100 ms", m_dmaSample.getRemaining() > 5);
  }

  @Test
  public void testPwmTimedTriggers() {
    m_dma.clearExternalTriggers();
    m_dma.setPwmEdgeTrigger(m_pwm, true, false);
    m_dma.start(1024);
    Timer.delay(Units.millisecondsToSeconds(100));
    m_dma.setPause(true);

    var timedOut = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(1));
    assertEquals(DMASample.DMAReadStatus.kOk, timedOut);
    assertTrue("Received more then 5 samples in 100 ms", m_dmaSample.getRemaining() > 5);
  }
}
