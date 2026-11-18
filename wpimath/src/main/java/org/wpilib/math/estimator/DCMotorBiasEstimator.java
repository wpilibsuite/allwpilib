// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

/**
 * This class estimates the input bias required to make the expected motor velocity match the actual
 * velocity over one timestep.
 *
 * <p>Subtracting this input bias from the next control input will cancel it and eliminate
 * steady-state error. This method of fixing steady-state error is superior to a pure integral
 * controller because it only integrates for unmodeled disturbances, not during expected transient
 * behavior while converging to a reference. This means there won't be overshoot during transients.
 *
 * <p>The feedforward gains passed to this class should match the ones used by the mechanism's
 * feedforward. The estimator estimates the error in that feedforward model; if the gains differ,
 * the difference shows up as input bias. In particular, errors in kV appear as an input bias
 * proportional to velocity, and errors in kA appear as an input bias during acceleration. Logging
 * the input bias estimate against velocity is a useful way to find these: a slope indicates kV
 * error, and a sign flip on direction reversal indicates kS error.
 */
public class DCMotorBiasEstimator {
  private final double m_ks;
  private final double m_kg;

  // Discrete velocity model
  private final double m_A;
  private final double m_B;

  // Observer gains
  private final double m_l1;
  private final double m_l2;

  // State estimate
  private double m_velocity;
  private double m_inputBias;

  /**
   * Constructs a DC motor bias estimator.
   *
   * <p>The model is
   *
   * <pre>
   * a = −kV/kA·v + 1/kA·(u + d) − kS/kA·sgn(v) − kG/kA
   * </pre>
   *
   * <p>where a is the acceleration, v is the velocity, u is the applied voltage, and d is the
   * estimated input bias.
   *
   * <p>For an arm, whose gravity term depends on angle, pass zero for kG and subtract the gravity
   * feedforward voltage from the voltage passed to {@link #calculate(double, double)} instead.
   *
   * @param ks The static gain in volts. This should match the feedforward's kS, or be zero if the
   *     feedforward doesn't use one.
   * @param kg The gravity gain in volts. This should match the feedforward's kG, or be zero if the
   *     feedforward doesn't use one.
   * @param kv The velocity gain in V/(units/s). Must be non-negative.
   * @param ka The acceleration gain in V/(units/s²). Must be non-negative. kV and kA can't both be
   *     zero.
   * @param timeConstant The approximate time in seconds for the input bias estimate to converge to
   *     a new constant input bias. Larger values reject disturbances more slowly but react less to
   *     model error and measurement noise. Must be positive.
   * @param dt Nominal discretization timestep in seconds. Must be positive.
   * @throws IllegalArgumentException if any of the parameter constraints are violated.
   */
  public DCMotorBiasEstimator(
      double ks, double kg, double kv, double ka, double timeConstant, double dt) {
    if (kv < 0.0) {
      throw new IllegalArgumentException("kv must be non-negative, got " + kv + "!");
    }
    if (ka < 0.0) {
      throw new IllegalArgumentException("ka must be non-negative, got " + ka + "!");
    }
    if (kv == 0.0 && ka == 0.0) {
      throw new IllegalArgumentException("kv and ka can't both be zero!");
    }
    if (timeConstant <= 0.0) {
      throw new IllegalArgumentException(
          "Time constant must be positive, got " + timeConstant + "!");
    }
    if (dt <= 0.0) {
      throw new IllegalArgumentException("dt must be positive, got " + dt + "!");
    }

    m_ks = ks;
    m_kg = kg;

    // Discretize the velocity dynamics v' = −kV/kA v + 1/kA (u + d) with a
    // zero-order hold. See wpimath/docs/SimpleMotorFeedforward.md for the
    // derivation, including the kA = 0 and kV = 0 cases.
    if (ka == 0.0) {
      m_A = 0.0;
      m_B = 1.0 / kv;
    } else if (kv == 0.0) {
      m_A = 1.0;
      m_B = dt / ka;
    } else {
      // B_d = A⁻¹(eᴬᵀ − 1)B simplifies to (1 − A_d)/kV
      double exponent = -kv / ka * dt;
      m_A = Math.exp(exponent);
      m_B = -Math.expm1(exponent) / kv;
    }

    // The augmented system is
    //
    //   xₖ₊₁ = Φxₖ + Γuₖ
    //   yₖ = Cxₖ
    //
    // where
    //
    //   x = [v]  Φ = [A_d  B_d]  Γ = [B_d]  C = [1  0]
    //       [d]      [ 0    1 ]      [ 0 ]
    //
    // The predictor-form observer is
    //
    //   x̂ₖ₊₁ = Φx̂ₖ + Γuₖ + L(yₖ − Cx̂ₖ)
    //
    // where L = [l₁  l₂]ᵀ. Subtracting it from the augmented system gives the
    // estimation error dynamics
    //
    //   x̃ₖ₊₁ = (Φ − LC)x̃ₖ
    //
    // where x̃ = x − x̂ and
    //
    //   Φ − LC = [A_d − l₁  B_d]
    //            [  −l₂      1 ]
    //
    // The error dynamics' poles are the roots of the characteristic polynomial
    //
    //   det(zI − (Φ − LC)) = det([z − A_d + l₁   −B_d ])
    //                            ([    l₂       z − 1 ])
    //                      = (z − A_d + l₁)(z − 1) + B_d l₂
    //                      = z² − (1 + A_d − l₁)z + (A_d − l₁ + B_d l₂)
    //
    // Matching coefficients with the desired characteristic polynomial
    //
    //   (z − p_v)(z − p_d) = z² − (p_v + p_d)z + p_v p_d
    //
    // gives
    //
    //   1 + A_d − l₁ = p_v + p_d
    //   A_d − l₁ + B_d l₂ = p_v p_d
    //
    // The first equation gives l₁. Substituting A_d − l₁ = p_v + p_d − 1 from
    // the first equation into the second gives l₂.
    //
    //   l₁ = 1 + A_d − p_v − p_d
    //   B_d l₂ = p_v p_d − p_v − p_d + 1
    //   l₂ = (1 − p_v)(1 − p_d)/B_d
    //
    // The input bias pole p_d uses the given time constant. The velocity pole
    // p_v is placed 5x faster so the input bias estimate's convergence time is
    // dominated by its own time constant.
    double p_d = Math.exp(-dt / timeConstant);
    double p_v = Math.exp(-dt / (timeConstant / 5.0));
    m_l1 = 1.0 + m_A - p_v - p_d;
    m_l2 = (1.0 - p_v) * (1.0 - p_d) / m_B;
  }

  /**
   * Constructs a DC motor bias estimator with a nominal timestep of 20 ms.
   *
   * @param ks The static gain in volts. This should match the feedforward's kS, or be zero if the
   *     feedforward doesn't use one.
   * @param kg The gravity gain in volts. This should match the feedforward's kG, or be zero if the
   *     feedforward doesn't use one.
   * @param kv The velocity gain in V/(units/s). Must be non-negative.
   * @param ka The acceleration gain in V/(units/s²). Must be non-negative. kV and kA can't both be
   *     zero.
   * @param timeConstant The approximate time in seconds for the input bias estimate to converge to
   *     a new constant input bias. Must be positive.
   * @throws IllegalArgumentException if any of the parameter constraints are violated.
   */
  public DCMotorBiasEstimator(double ks, double kg, double kv, double ka, double timeConstant) {
    this(ks, kg, kv, ka, timeConstant, 0.020);
  }

  /**
   * Constructs a DC motor bias estimator for a mechanism without a gravity term, such as a flywheel
   * or drivetrain, with a nominal timestep of 20 ms.
   *
   * <p>The model is
   *
   * <pre>
   * a = −kV/kA·v + 1/kA·(u + d) − kS/kA·sgn(v)
   * </pre>
   *
   * <p>where a is the acceleration, v is the velocity, u is the applied voltage, and d is the
   * estimated input bias.
   *
   * @param ks The static gain in volts. This should match the feedforward's kS, or be zero if the
   *     feedforward doesn't use one.
   * @param kv The velocity gain in V/(units/s). Must be non-negative.
   * @param ka The acceleration gain in V/(units/s²). Must be non-negative. kV and kA can't both be
   *     zero.
   * @param timeConstant The approximate time in seconds for the input bias estimate to converge to
   *     a new constant input bias. Must be positive.
   * @throws IllegalArgumentException if any of the parameter constraints are violated.
   */
  public DCMotorBiasEstimator(double ks, double kv, double ka, double timeConstant) {
    this(ks, 0.0, kv, ka, timeConstant, 0.020);
  }

  /**
   * Returns the input bias estimate.
   *
   * @param velocity The velocity from the current timestep.
   * @param voltage The voltage applied for the current timestep.
   * @return The input bias estimate in volts.
   */
  public double calculate(double velocity, double voltage) {
    // Voltage left over after the static friction and gravity terms
    double u = voltage - m_ks * Math.signum(velocity) - m_kg;

    double residual = velocity - m_velocity;
    m_velocity = m_A * m_velocity + m_B * (u + m_inputBias) + m_l1 * residual;
    m_inputBias += m_l2 * residual;

    return m_inputBias;
  }

  /**
   * Resets the estimator to the given velocity with no input bias.
   *
   * @param velocity The current velocity.
   */
  public void reset(double velocity) {
    m_velocity = velocity;
    m_inputBias = 0.0;
  }
}
