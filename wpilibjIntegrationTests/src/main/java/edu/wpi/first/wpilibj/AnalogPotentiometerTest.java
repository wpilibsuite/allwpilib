/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertEquals;

/**
 * Tests the {@link AnalogPotentiometer}.
 */
public class AnalogPotentiometerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(AnalogPotentiometerTest.class.getName());
  private AnalogCrossConnectFixture m_analogIO;
  private FakePotentiometerSource m_potSource;
  private AnalogPotentiometer m_pot;

  private static final double DOUBLE_COMPARISON_DELTA = 2.0;

  @Before
  public void setUp() throws Exception {
    m_analogIO = TestBench.getAnalogCrossConnectFixture();
    m_potSource = new FakePotentiometerSource(m_analogIO.getOutput(), 360);
    m_pot = new AnalogPotentiometer(m_analogIO.getInput(), 360.0, 0);

  }

  @After
  public void tearDown() throws Exception {
    m_potSource.reset();
    m_pot.free();
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
      m_potSource.setMaxVoltage(ControllerPower.getVoltage5V());
      Timer.delay(.02);
      assertEquals(i, m_pot.get(), DOUBLE_COMPARISON_DELTA);
    }
  }


}
