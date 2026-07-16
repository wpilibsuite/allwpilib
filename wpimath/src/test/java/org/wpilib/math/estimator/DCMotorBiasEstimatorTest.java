// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class DCMotorBiasEstimatorTest {
  private static final double ks = 0.5;
  private static final double kg = 1.0;
  private static final double kv = 1.0;
  private static final double ka = 0.1;
  private static final double dt = 0.005; // s
  private static final double duration = 2.0; // s

  // Continuous DC motor model
  private static final double A_c = -kv / ka;
  private static final double B_c = 1 / ka;

  // Discrete DC motor model
  private static final double A_d = Math.exp(A_c * dt);
  private static final double B_d = 1 / A_c * (A_d - 1) * B_c;

  private final DCMotorBiasEstimator m_estimator =
      new DCMotorBiasEstimator(ks, kg, kv, ka, 0.05, dt);
  private double m_v;
  private double m_uBiasEstimate;

  private void simulate(double u, double uBias) {
    for (int i = 0; i < duration / dt; ++i) {
      m_uBiasEstimate = m_estimator.calculate(m_v, u);
      m_v = A_d * m_v + B_d * (u - ks * Math.signum(m_v) - kg + uBias);
    }
  }

  @Test
  void testConstantBias() {
    m_estimator.reset(m_v);

    // Positive bias
    simulate(5.0, 2.0);
    assertEquals((5.0 - ks - kg + 2.0) / kv, m_v, 1e-6);
    assertEquals(2.0, m_uBiasEstimate, 1e-6);

    // Negative bias
    simulate(5.0, -2.0);
    assertEquals((5.0 - ks - kg - 2.0) / kv, m_v, 1e-6);
    assertEquals(-2.0, m_uBiasEstimate, 1e-6);

    // Zero bias
    simulate(5.0, 0.0);
    assertEquals((5.0 - ks - kg) / kv, m_v, 1e-6);
    assertEquals(0.0, m_uBiasEstimate, 1e-6);

    // Reverse direction with positive bias
    simulate(-5.0, 1.0);
    assertEquals((-5.0 + ks - kg + 1.0) / kv, m_v, 1e-6);
    assertEquals(1.0, m_uBiasEstimate, 1e-6);
  }

  @Test
  void testPureInertia() {
    var estimator = new DCMotorBiasEstimator(0.0, 0.0, 0.0, ka, 0.05, dt);
    double v = 0.0;
    double uBiasEstimate = 0.0;

    estimator.reset(v);

    // With no back-EMF, the velocity ramps without bound, but the estimator
    // should still find the bias
    final double uBias = 2.0;
    for (int i = 0; i < duration / dt; ++i) {
      uBiasEstimate = estimator.calculate(v, 1.0);
      v += dt / ka * (1.0 + uBias);
    }
    assertEquals(uBias, uBiasEstimate, 1e-6);
  }

  @Test
  void testNoGravityOverload() {
    var withoutKg = new DCMotorBiasEstimator(ks, kv, ka, 0.05);
    var withZeroKg = new DCMotorBiasEstimator(ks, 0.0, kv, ka, 0.05);

    withoutKg.reset(0.0);
    withZeroKg.reset(0.0);

    for (int i = -10; i < 10; ++i) {
      double v = i * 0.1;
      assertEquals(withZeroKg.calculate(v, 5.0), withoutKg.calculate(v, 5.0));
    }
  }

  @Test
  void testInvalidArguments() {
    assertAll(
        () ->
            assertThrows(
                IllegalArgumentException.class,
                () -> new DCMotorBiasEstimator(0.0, 0.0, -1.0, 0.1, 0.05)),
        () ->
            assertThrows(
                IllegalArgumentException.class,
                () -> new DCMotorBiasEstimator(0.0, 0.0, 1.0, -0.1, 0.05)),
        () ->
            assertThrows(
                IllegalArgumentException.class,
                () -> new DCMotorBiasEstimator(0.0, 0.0, 0.0, 0.0, 0.05)),
        () ->
            assertThrows(
                IllegalArgumentException.class,
                () -> new DCMotorBiasEstimator(0.0, 0.0, 1.0, 0.1, 0.0)),
        () ->
            assertThrows(
                IllegalArgumentException.class,
                () -> new DCMotorBiasEstimator(0.0, 0.0, 1.0, 0.1, 0.05, 0.0)));
  }
}
