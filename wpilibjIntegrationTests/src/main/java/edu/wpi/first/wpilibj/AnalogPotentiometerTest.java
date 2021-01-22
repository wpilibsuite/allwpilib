// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.logging.Logger;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/** Tests the {@link AnalogPotentiometer}. */
public class AnalogPotentiometerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(AnalogPotentiometerTest.class.getName());
  private AnalogCrossConnectFixture m_analogIO;
  private FakePotentiometerSource m_potSource;
  private AnalogPotentiometer m_pot;

  private static final double DOUBLE_COMPARISON_DELTA = 2.0;

  @Before
  public void setUp() {
    m_analogIO = TestBench.getAnalogCrossConnectFixture();
    m_potSource = new FakePotentiometerSource(m_analogIO.getOutput(), 360);
    m_pot = new AnalogPotentiometer(m_analogIO.getInput(), 360.0, 0);
  }

  @After
  public void tearDown() {
    m_potSource.reset();
    m_pot.close();
    m_analogIO.teardown();
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testInitialSettings() {
    assertEquals(0, m_pot.get(), DOUBLE_COMPARISON_DELTA);
  }

  @Test
  public void testRangeValues() {
    for (double i = 0.0; i < 360.0; i = i + 1.0) {
      m_potSource.setAngle(i);
      m_potSource.setMaxVoltage(RobotController.getVoltage5V());
      Timer.delay(0.02);
      assertEquals(i, m_pot.get(), DOUBLE_COMPARISON_DELTA);
    }
  }
}
