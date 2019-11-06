/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>
#include <wpi/MathExtras.h>

namespace frc {
template <class Velocity>
class SimpleMotorFeedforward {
  using Acceleration =
      units::unit_t<units::compound_unit<typename Velocity::unit_type,
                                         units::inverse<units::seconds>>>;
  using kv_unit_t = units::unit_t<units::compound_unit<
      units::volts, units::inverse<typename Velocity::unit_type>>>;
  using ka_unit_t = units::unit_t<units::compound_unit<
      units::volts, units::inverse<typename Acceleration::unit_type>>>;

 public:
  SimpleMotorFeedforward() : m_kS(0), m_kV(0), m_kA(0) {}

  SimpleMotorFeedforward(units::volt_t kS, kv_unit_t kV,
                         ka_unit_t kA = ka_unit_t(0))
      : m_kS(kS), m_kV(kV), m_kA(kA) {}

  units::volt_t Calculate(Velocity velocity,
                          Acceleration acceleration = Acceleration(0)) {
    return m_kS * wpi::sgn(velocity) + m_kV * velocity + m_kA * acceleration;
  }

 private:
  const units::volt_t m_kS;
  const kv_unit_t m_kV;
  const ka_unit_t m_kA;
};
}  // namespace frc
