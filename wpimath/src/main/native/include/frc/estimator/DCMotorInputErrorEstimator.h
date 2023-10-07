// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/EigenCore.h"
#include "frc/estimator/KalmanFilter.h"
#include "units/acceleration.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {

/**
 * This class estimates the input error required to make the expected motor
 * position match the actual position over one timestep.
 *
 * Subtracting this input error from the next control input will correct for the
 * bias and eliminate steady-state error. This method of fixing steady-state
 * error is superior to a pure integral controller because it only integrates
 * for unmodeled disturbances, not during expected transient behavior while
 * converging to a reference. This means there won't be overshoot during
 * transients.
 */
template <class Distance>
  requires units::traits::is_length_unit_v<Distance> ||
           units::traits::is_angle_unit_v<Distance>
class DCMotorInputErrorEstimator {
 public:
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;

  using Kv_t = units::unit_t<
      units::compound_unit<units::volt, units::inverse<Velocity>>>;
  using Ka_t = units::unit_t<
      units::compound_unit<units::volt, units::inverse<Acceleration>>>;

  /**
   * Constructs a DC motor input error estimator.
   *
   * @param Kv The velocity feedforward gain.
   * @param Ka The acceleration feedforward gain.
   * @param dt Nominal discretization timestep.
   */
  DCMotorInputErrorEstimator(Kv_t Kv, Ka_t Ka, units::second_t dt)
      : m_dt{dt},
        m_system{[&] {
          Matrixd<3, 3> A{{0, 1, 0},
                          {0, -Kv.value() / Ka.value(), 1 / Ka.value()},
                          {0, 0, 0}};
          Matrixd<3, 1> B{{0, 1 / Ka.value(), 0}};
          Matrixd<1, 3> C{{1, 0, 0}};
          Matrixd<1, 1> D{0};

          return LinearSystem<3, 1, 1>{A, B, C, D};
        }()},
        m_kf{m_system, {0.01, 0.1, 12.0}, {0.01}, m_dt} {}

  /**
   * Returns the input error estimate.
   *
   * @param oldInput The control input applied in the previous timestep.
   * @param currentPosition The current position.
   */
  units::volt_t Calculate(units::volt_t oldInput, Distance_t currentPosition) {
    m_kf.Predict(Vectord<1>{oldInput.value()}, m_dt);
    m_kf.Correct(Vectord<1>{oldInput.value()},
                 Vectord<1>{currentPosition.value()});

    return units::volt_t{m_kf.Xhat(2)};
  }

  /**
   * Resets the estimator to the given position and velocity with no input
   * error.
   *
   * @param currentPosition The current position.
   * @param currentVelocity The current velocity.
   */
  void Reset(Distance_t currentPosition, Velocity_t currentVelocity) {
    m_kf.SetXhat(
        Vectord<3>{currentPosition.value(), currentVelocity.value(), 0.0});
  }

 private:
  units::second_t m_dt;
  LinearSystem<3, 1, 1> m_system;
  KalmanFilter<3, 1, 1> m_kf;
};

}  // namespace frc
