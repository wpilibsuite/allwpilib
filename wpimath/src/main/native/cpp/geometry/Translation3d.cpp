// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Translation3d.h"

#include <wpi/json.h>

#include "units/length.h"
#include "units/math.h"

using namespace frc;

Translation3d::Translation3d(units::meter_t distance, const Rotation3d& angle) {
  auto rectangular = Translation3d{distance, 0_m, 0_m}.RotateBy(angle);
  m_x = rectangular.X();
  m_y = rectangular.Y();
  m_z = rectangular.Z();
}

units::meter_t Translation3d::Distance(const Translation3d& other) const {
  return units::math::sqrt(units::math::pow<2>(other.m_x - m_x) +
                           units::math::pow<2>(other.m_y - m_y) +
                           units::math::pow<2>(other.m_z - m_z));
}

units::meter_t Translation3d::Norm() const {
  return units::math::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

Translation3d Translation3d::RotateBy(const Rotation3d& other) const {
  Quaternion p{0.0, m_x.value(), m_y.value(), m_z.value()};
  auto qprime = other.GetQuaternion() * p * other.GetQuaternion().Inverse();
  return Translation3d{units::meter_t{qprime.X()}, units::meter_t{qprime.Y()},
                       units::meter_t{qprime.Z()}};
}

bool Translation3d::operator==(const Translation3d& other) const {
  return units::math::abs(m_x - other.m_x) < 1E-9_m &&
         units::math::abs(m_y - other.m_y) < 1E-9_m &&
         units::math::abs(m_z - other.m_z) < 1E-9_m;
}

void frc::to_json(wpi::json& json, const Translation3d& translation) {
  json = wpi::json{{"x", translation.X().value()},
                   {"y", translation.Y().value()},
                   {"z", translation.Z().value()}};
}

void frc::from_json(const wpi::json& json, Translation3d& translation) {
  translation = Translation3d{units::meter_t{json.at("x").get<double>()},
                              units::meter_t{json.at("y").get<double>()},
                              units::meter_t{json.at("z").get<double>()}};
}
