/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.ctrlsys.INode;
import edu.wpi.first.wpilibj.ctrlsys.PIDController;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class PIDToleranceTest {
  private PIDController m_pid;
  private final double m_setPoint = 50.0;
  private final double m_tolerance = 10.0;
  private final double m_range = 200;

  @BeforeClass
  public static void setupClass() {
    UnitTestUtility.setupMockBase();
  }

  private class FakeInput implements INode {
    public double m_val;

    FakeInput() {
      m_val = 0;
    }

    @Override
    public double getOutput() {
      return m_val;
    }
  }

  private FakeInput m_inp;
  private PIDOutput m_out = new PIDOutput() {
    @Override
    public void pidWrite(double out) {
    }

  };


  @Before
  public void setUp() throws Exception {
    m_inp = new FakeInput();
    m_pid = new PIDController(0.05, 0.0, 0.0, m_inp, m_out);
    m_pid.setInputRange(-m_range / 2, m_range / 2);
  }

  @After
  public void tearDown() throws Exception {
    m_pid.free();
    m_pid = null;
  }

  @Test
  public void testAbsoluteTolerance() {
    m_pid.setAbsoluteTolerance(m_tolerance);
    m_pid.setSetpoint(m_setPoint);
    m_pid.enable();
    Timer.delay(1);
    assertFalse("Error was in tolerance when it should not have been. Error was "
        + m_pid.getError(), m_pid.onTarget());
    m_inp.m_val = m_setPoint + m_tolerance / 2;
    Timer.delay(1.0);
    assertTrue("Error was not in tolerance when it should have been. Error was "
        + m_pid.getError(), m_pid.onTarget());
    m_inp.m_val = m_setPoint + 10 * m_tolerance;
    Timer.delay(1.0);
    assertFalse("Error was in tolerance when it should not have been. Error was "
        + m_pid.getError(), m_pid.onTarget());
  }
}
