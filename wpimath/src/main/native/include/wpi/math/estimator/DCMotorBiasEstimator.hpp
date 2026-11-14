// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include "wpi/units/angle.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/dimensionless.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/MathExtras.hpp"

namespace wpi::math {

/**
 * This class estimates the input bias required to make the expected motor
 * velocity match the actual velocity over one timestep.
 *
 * Subtracting this input bias from the next control input will cancel it and
 * eliminate steady-state error. This method of fixing steady-state error is
 * superior to a pure integral controller because it only integrates for
 * unmodeled disturbances, not during expected transient behavior while
 * converging to a reference. This means there won't be overshoot during
 * transients.
 *
 * The feedforward gains passed to this class should match the ones used by the
 * mechanism's feedforward. The estimator estimates the error in that
 * feedforward model; if the gains differ, the difference shows up as input
 * error. In particular, errors in kV appear as an input bias proportional to
 * velocity, and errors in kA appear as an input bias during acceleration.
 * Logging the input bias estimate against velocity is a useful way to find
 * these: a slope indicates kV error, and a sign flip on direction reversal
 * indicates kS error.
 */
template <class Distance>
  requires wpi::units::length_unit<Distance> ||
           wpi::units::angle_unit<Distance> ||
           wpi::units::dimensionless_unit<Distance>
class DCMotorBiasEstimator {
 public:
  using Velocity =
      wpi::units::compound_unit<Distance, wpi::units::inverse<units::seconds>>;
  using Velocity_t = wpi::units::unit_t<Velocity>;
  using Acceleration =
      wpi::units::compound_unit<Velocity, wpi::units::inverse<units::seconds>>;

  using kV_t = wpi::units::unit_t<
      wpi::units::compound_unit<units::volt, wpi::units::inverse<Velocity>>>;
  using kA_t = wpi::units::unit_t<wpi::units::compound_unit<
      units::volt, wpi::units::inverse<Acceleration>>>;
  using VelocityPerVolt_t = wpi::units::unit_t<
      wpi::units::compound_unit<Velocity, wpi::units::inverse<units::volt>>>;

  /**
   * Constructs a DC motor bias estimator.
   *
   * The model is
   *
   * <pre>
   * a = −kV/kA·v + 1/kA·(u + d) − kS/kA·sgn(v) − kG/kA
   * </pre>
   *
   * where a is the acceleration, v is the velocity, u is the applied voltage,
   * and d is the estimated input bias.
   *
   * For an arm, whose gravity term depends on angle, pass zero for kG and
   * subtract the gravity feedforward voltage from the voltage passed to
   * Calculate() instead.
   *
   * @param kS The static gain. This should match the feedforward's kS, or be
   *     zero if the feedforward doesn't use one.
   * @param kG The gravity gain. This should match the feedforward's kG, or be
   *     zero if the feedforward doesn't use one.
   * @param kV The velocity gain. Must be non-negative.
   * @param kA The acceleration gain. Must be non-negative. kV and kA can't
   *     both be zero.
   * @param timeConstant The approximate time for the input bias estimate to
   *     converge to a new constant input bias. Larger values reject
   *     disturbances more slowly but react less to model error and
   *     measurement noise. Must be positive.
   * @param dt Nominal discretization timestep. Must be positive.
   * @throws std::invalid_argument if any of the parameter constraints are
   *     violated.
   */
  constexpr DCMotorBiasEstimator(wpi::units::volt_t kS, wpi::units::volt_t kG,
                                 kV_t kV, kA_t kA,
                                 wpi::units::second_t timeConstant,
                                 wpi::units::second_t dt = 20_ms)
      : m_kS{kS}, m_kG{kG} {
    if (kV < kV_t{0}) {
      throw std::invalid_argument("kV must be non-negative");
    }
    if (kA < kA_t{0}) {
      throw std::invalid_argument("kA must be non-negative");
    }
    if (kV == kV_t{0} && kA == kA_t{0}) {
      throw std::invalid_argument("kV and kA can't both be zero");
    }
    if (timeConstant <= 0_s) {
      throw std::invalid_argument("Time constant must be positive");
    }
    if (dt <= 0_s) {
      throw std::invalid_argument("dt must be positive");
    }

    // Discretize the velocity dynamics v' = −kV/kA v + 1/kA (u + d) with a
    // zero-order hold. See wpimath/docs/SimpleMotorFeedforward.md for the
    // derivation, including the kA = 0 and kV = 0 cases.
    if (kA == kA_t{0}) {
      m_A = 0.0;
      m_B = 1.0 / kV;
    } else if (kV == kV_t{0}) {
      m_A = 1.0;
      m_B = dt / kA;
    } else {
      // B_d = A⁻¹(eᴬᵀ − 1)B simplifies to (1 − A_d)/kV
      auto exponent = -kV / kA * dt;
      m_A = wpi::units::math::exp(exponent);
      m_B = -wpi::units::math::expm1(exponent) / kV;
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
    wpi::units::dimensionless_t p_d = wpi::units::math::exp(-dt / timeConstant);
    wpi::units::dimensionless_t p_v =
        wpi::units::math::exp(-dt / (timeConstant / 5.0));
    m_l1 = 1.0 + m_A - p_v - p_d;
    m_l2 = (1.0 - p_v) * (1.0 - p_d) / m_B;
  }

  /**
   * Constructs a DC motor bias estimator for a mechanism without a gravity
   * term, such as a flywheel or drivetrain.
   *
   * The model is
   *
   * <pre>
   * a = −kV/kA·v + 1/kA·(u + d) − kS/kA·sgn(v)
   * </pre>
   *
   * where a is the acceleration, v is the velocity, u is the applied voltage,
   * and d is the estimated input bias.
   *
   * @param kS The static gain. This should match the feedforward's kS, or be
   *     zero if the feedforward doesn't use one.
   * @param kV The velocity gain. Must be non-negative.
   * @param kA The acceleration gain. Must be non-negative. kV and kA can't
   *     both be zero.
   * @param timeConstant The approximate time for the input bias estimate to
   *     converge to a new constant input bias. Larger values reject
   *     disturbances more slowly but react less to model error and
   *     measurement noise. Must be positive.
   * @param dt Nominal discretization timestep. Must be positive.
   * @throws std::invalid_argument if any of the parameter constraints are
   *     violated.
   */
  constexpr DCMotorBiasEstimator(wpi::units::volt_t kS, kV_t kV, kA_t kA,
                                 wpi::units::second_t timeConstant,
                                 wpi::units::second_t dt = 20_ms)
      : DCMotorBiasEstimator{kS, 0_V, kV, kA, timeConstant, dt} {}

  /**
   * Returns the input bias estimate.
   *
   * @param velocity The velocity from the current timestep.
   * @param voltage The voltage applied for the current timestep.
   */
  constexpr wpi::units::volt_t Calculate(Velocity_t velocity,
                                         units::volt_t voltage) {
    // Voltage left over after the static friction and gravity terms
    wpi::units::volt_t u = voltage - m_kS * wpi::util::sgn(velocity) - m_kG;

    Velocity_t residual = velocity - m_velocity;
    m_velocity = m_A * m_velocity + m_B * (u + m_inputBias) + m_l1 * residual;
    m_inputBias += m_l2 * residual;

    return m_inputBias;
  }

  /**
   * Resets the estimator to the given velocity with no input bias.
   *
   * @param velocity The current velocity.
   */
  constexpr void Reset(Velocity_t velocity) {
    m_velocity = velocity;
    m_inputBias = 0_V;
  }

 private:
  wpi::units::volt_t m_kS;
  wpi::units::volt_t m_kG;

  // Discrete velocity model
  wpi::units::dimensionless_t m_A{0};
  VelocityPerVolt_t m_B{0};

  // Observer gains
  wpi::units::dimensionless_t m_l1{0};
  kV_t m_l2{0};

  // State estimate
  Velocity_t m_velocity{0};
  wpi::units::volt_t m_inputBias{0};
};

}  // namespace wpi::math
