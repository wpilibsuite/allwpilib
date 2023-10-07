// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.system.LinearSystem;

/**
 * This class estimates the input error required to make the expected motor position match the
 * actual position over one timestep.
 *
 * <p>Subtracting this input error from the next control input will correct for the bias and
 * eliminate steady-state error. This method of fixing steady-state error is superior to a pure
 * integral controller because it only integrates for unmodeled disturbances, not during expected
 * transient behavior while converging to a reference. This means there won't be overshoot during
 * transients.
 */
public class DCMotorInputErrorEstimator {
  private final double m_dt;
  private final KalmanFilter<N3, N1, N1> m_kf;

  /**
   * Constructs a DC motor input error estimator.
   *
   * @param Kv The velocity feedforward gain.
   * @param Ka The acceleration feedforward gain.
   * @param dt Nominal discretization timestep in seconds.
   */
  public DCMotorInputErrorEstimator(double Kv, double Ka, double dt) {
    m_dt = dt;

    var A = new Matrix<>(Nat.N3(), Nat.N3(), new double[] {0, 1, 0, 0, -Kv / Ka, 1 / Ka, 0, 0, 0});
    var B = new Matrix<>(Nat.N3(), Nat.N1(), new double[] {0, 1 / Ka, 0});
    var C = new Matrix<>(Nat.N1(), Nat.N3(), new double[] {1, 0, 0});
    var D = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0});
    var system = new LinearSystem<N3, N1, N1>(A, B, C, D);
    m_kf =
        new KalmanFilter<N3, N1, N1>(
            Nat.N3(),
            Nat.N1(),
            system,
            VecBuilder.fill(0.01, 0.1, 12.0),
            VecBuilder.fill(0.01),
            m_dt);
  }

  /**
   * Returns the input error estimate.
   *
   * @param oldInputVolts The control input applied in the previous timestep.
   * @param currentPosition The current position.
   * @return The input error estimate.
   */
  public double calculate(double oldInputVolts, double currentPosition) {
    m_kf.predict(VecBuilder.fill(oldInputVolts), m_dt);
    m_kf.correct(VecBuilder.fill(oldInputVolts), VecBuilder.fill(currentPosition));

    return m_kf.getXhat(2);
  }

  /**
   * Resets the estimator to the given position and velocity with no input error.
   *
   * @param currentPosition The current position.
   * @param currentVelocity The current velocity.
   */
  public void reset(double currentPosition, double currentVelocity) {
    m_kf.setXhat(VecBuilder.fill(currentPosition, currentVelocity, 0.0));
  }
}
