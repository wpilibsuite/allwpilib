// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.system.plant.DCMotor;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

class ExtendedKalmanFilterTest {
  private static Matrix<N5, N1> getDynamics(final Matrix<N5, N1> x, final Matrix<N2, N1> u) {
    final var motors = DCMotor.getCIM(2);

    final var gr = 7.08; // Gear ratio
    final var rb = 0.8382 / 2.0; // Wheelbase radius (trackwidth)
    final var r = 0.0746125; // Wheel radius
    final var m = 63.503; // Robot mass
    final var J = 5.6; // Robot moment of inertia

    final var C1 = -Math.pow(gr, 2) * motors.Kt / (motors.Kv * motors.R * r * r);
    final var C2 = gr * motors.Kt / (motors.R * r);
    final var k1 = 1.0 / m + rb * rb / J;
    final var k2 = 1.0 / m - rb * rb / J;

    final var vl = x.get(3, 0);
    final var vr = x.get(4, 0);
    final var Vl = u.get(0, 0);
    final var Vr = u.get(1, 0);

    final Matrix<N5, N1> result = new Matrix<>(Nat.N5(), Nat.N1());
    final var v = 0.5 * (vl + vr);
    result.set(0, 0, v * Math.cos(x.get(2, 0)));
    result.set(1, 0, v * Math.sin(x.get(2, 0)));
    result.set(2, 0, (vr - vl) / (2.0 * rb));
    result.set(3, 0, k1 * ((C1 * vl) + (C2 * Vl)) + k2 * ((C1 * vr) + (C2 * Vr)));
    result.set(4, 0, k2 * ((C1 * vl) + (C2 * Vl)) + k1 * ((C1 * vr) + (C2 * Vr)));
    return result;
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @Test
  void testInit() {
    double dt = 0.00505;

    assertDoesNotThrow(
        () -> {
          ExtendedKalmanFilter<N5, N2, N3> observer =
              new ExtendedKalmanFilter<>(
                  Nat.N5(),
                  Nat.N2(),
                  Nat.N3(),
                  ExtendedKalmanFilterTest::getDynamics,
                  ExtendedKalmanFilterTest::getLocalMeasurementModel,
                  VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                  VecBuilder.fill(0.0001, 0.01, 0.01),
                  dt);

          Matrix<N2, N1> u = VecBuilder.fill(12.0, 12.0);
          observer.predict(u, dt);

          var localY = getLocalMeasurementModel(observer.getXhat(), u);
          observer.correct(u, localY);

          var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
          var R = StateSpaceUtil.makeCostMatrix(VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
          observer.correct(
              Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
        });
  }

}
