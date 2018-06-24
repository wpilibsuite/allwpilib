/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PIDToleranceTest {
  private PIDController m_pid;
  private static final double m_setPoint = 50.0;
  private static final double m_tolerance = 10.0;
  private static final double m_range = 200;

  private class FakeInput implements PIDSource {
    public double m_val;

    FakeInput() {
      m_val = 0;
    }

    @Override
    public PIDSourceType getPIDSourceType() {
      return PIDSourceType.kDisplacement;
    }

    @Override
    public double pidGet() {
      return m_val;
    }

    @Override
    public void setPIDSourceType(PIDSourceType arg0) {
    }
  }

  private FakeInput m_inp;
  private final PIDOutput m_out = new PIDOutput() {
    @Override
    public void pidWrite(double out) {
    }

  };


  @BeforeEach
  void setUp() {
    m_inp = new FakeInput();
    m_pid = new PIDController(0.05, 0.0, 0.0, m_inp, m_out);
    m_pid.setInputRange(-m_range / 2, m_range / 2);
  }

  @AfterEach
  void tearDown() {
    m_pid.close();
    m_pid = null;
  }

  @Test
  void absoluteToleranceTest() {
    m_pid.setAbsoluteTolerance(m_tolerance);
    m_pid.setSetpoint(m_setPoint);
    m_pid.enable();
    Timer.delay(1);
    assertFalse(m_pid.onTarget(), "Error was in tolerance when it should not have been. Error was "
        + m_pid.getError());
    m_inp.m_val = m_setPoint + m_tolerance / 2;
    Timer.delay(1.0);
    assertTrue(m_pid.onTarget(), "Error was not in tolerance when it should have been. Error was "
        + m_pid.getError());
    m_inp.m_val = m_setPoint + 10 * m_tolerance;
    Timer.delay(1.0);
    assertFalse(m_pid.onTarget(), "Error was in tolerance when it should not have been. Error was "
        + m_pid.getError());
  }

  @Test
  void percentToleranceTest() {
    m_pid.setPercentTolerance(m_tolerance);
    m_pid.setSetpoint(m_setPoint);
    m_pid.enable();
    assertFalse(m_pid.onTarget(), "Error was in tolerance when it should not have been. Error was "
        + m_pid.getError());
    //half of percent tolerance away from setPoint
    m_inp.m_val = m_setPoint + m_tolerance / 200 * m_range;
    Timer.delay(1.0);
    assertTrue(m_pid.onTarget(), "Error was not in tolerance when it should have been. Error was "
        + m_pid.getError());
    //double percent tolerance away from setPoint
    m_inp.m_val = m_setPoint + m_tolerance / 50 * m_range;
    Timer.delay(1.0);
    assertFalse(m_pid.onTarget(), "Error was in tolerance when it should not have been. Error was "
        + m_pid.getError());
  }
}
