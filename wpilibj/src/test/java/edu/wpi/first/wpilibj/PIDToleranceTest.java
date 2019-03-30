/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PIDToleranceTest {
  private PIDController m_pidController;
  private AsynchronousControllerRunner m_pidRunner;
  private static final double m_reference = 50.0;
  private static final double m_tolerance = 10.0;
  private static final double m_range = 200;

  private static class FakeInput implements DoubleSupplier {
    public double m_val;

    FakeInput() {
      m_val = 0;
    }

    @Override
    public double getAsDouble() {
      return m_val;
    }
  }

  private FakeInput m_inp;
  private final DoubleConsumer m_out = new DoubleConsumer() {
    @Override
    public void accept(double value) {

    }
  };


  @BeforeEach
  void setUp() {
    m_inp = new FakeInput();
    m_pidController = new PIDController(0.05, 0.0, 0.0);
    m_pidRunner =
        new AsynchronousControllerRunner(m_pidController, () -> m_reference, m_inp, m_out);
    m_pidController.setInputRange(-m_range / 2, m_range / 2);
  }

  @AfterEach
  void tearDown() {
    m_pidController.close();
    m_pidController = null;
  }

  @Test
  void absoluteToleranceTest() {
    m_pidController.setAbsoluteTolerance(m_tolerance);
    m_pidRunner.enable();
    Timer.delay(1);
    assertFalse(m_pidController.atReference(),
        "Error was in tolerance when it should not have been. Error was "
            + m_pidController.getError());
    m_inp.m_val = m_reference + m_tolerance / 2;
    Timer.delay(1.0);
    assertTrue(m_pidController.atReference(),
        "Error was not in tolerance when it should have been. Error was "
            + m_pidController.getError());
    m_inp.m_val = m_reference + 10 * m_tolerance;
    Timer.delay(1.0);
    assertFalse(m_pidController.atReference(),
        "Error was in tolerance when it should not have been. Error was "
            + m_pidController.getError());
  }

  @Test
  void percentToleranceTest() {
    m_pidController.setPercentTolerance(m_tolerance);
    m_pidController.setReference(m_reference);
    m_pidRunner.enable();
    assertFalse(m_pidController.atReference(),
        "Error was in tolerance when it should not have been. Error was "
            + m_pidController.getError());
    //half of percent tolerance away from setPoint
    m_inp.m_val = m_reference + m_tolerance / 200 * m_range;
    Timer.delay(1.0);
    assertTrue(m_pidController.atReference(),
        "Error was not in tolerance when it should have been. Error was "
            + m_pidController.getError());
    //double percent tolerance away from setPoint
    m_inp.m_val = m_reference + m_tolerance / 50 * m_range;
    Timer.delay(1.0);
    assertFalse(m_pidController.atReference(),
        "Error was in tolerance when it should not have been. Error was "
            + m_pidController.getError());
  }
}
