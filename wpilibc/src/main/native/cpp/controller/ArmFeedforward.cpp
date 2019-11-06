/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/ArmFeedforward.h"

#include <wpi/MathExtras.h>

using namespace frc;

using Angle = units::radians;
using Velocity = units::radians_per_second;
using Acceleration = units::compound_unit<units::radians_per_second,
                                          units::inverse<units::second>>;
using kv_unit = units::compound_unit<units::volts,
                                     units::inverse<units::radians_per_second>>;
using ka_unit =
    units::compound_unit<units::volts, units::inverse<Acceleration>>;

ArmFeedforward::ArmFeedforward(units::volt_t kS, units::volt_t kCos,
                               units::unit_t<kv_unit> kV,
                               units::unit_t<ka_unit> kA)
    : m_kS(kS), m_kCos(kCos), m_kV(kV), m_kA(kA) {}

units::volt_t ArmFeedforward::Calculate(
    units::unit_t<Angle> angle, units::unit_t<Velocity> velocity,
    units::unit_t<Acceleration> acceleration) {
  return m_kS * wpi::sgn(velocity) + m_kCos * units::math::cos(angle) +
         m_kV * velocity + m_kA * acceleration;
}
