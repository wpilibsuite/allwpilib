// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Translation2d.h"

#include <wpi/json.h>

#include "geometry2d.pb.h"
#include "units/math.h"

using namespace frc;

Translation2d::Translation2d(const Eigen::Vector2d& vector)
    : m_x{units::meter_t{vector.x()}}, m_y{units::meter_t{vector.y()}} {}

units::meter_t Translation2d::Distance(const Translation2d& other) const {
  return units::math::hypot(other.m_x - m_x, other.m_y - m_y);
}

units::meter_t Translation2d::Norm() const {
  return units::math::hypot(m_x, m_y);
}

bool Translation2d::operator==(const Translation2d& other) const {
  return units::math::abs(m_x - other.m_x) < 1E-9_m &&
         units::math::abs(m_y - other.m_y) < 1E-9_m;
}

Translation2d Translation2d::Nearest(
    std::span<const Translation2d> translations) const {
  return *std::min_element(translations.begin(), translations.end(),
                           [this](Translation2d a, Translation2d b) {
                             return this->Distance(a) < this->Distance(b);
                           });
}

Translation2d Translation2d::Nearest(
    std::initializer_list<Translation2d> translations) const {
  return *std::min_element(translations.begin(), translations.end(),
                           [this](Translation2d a, Translation2d b) {
                             return this->Distance(a) < this->Distance(b);
                           });
}

void frc::to_json(wpi::json& json, const Translation2d& translation) {
  json =
      wpi::json{{"x", translation.X().value()}, {"y", translation.Y().value()}};
}

void frc::from_json(const wpi::json& json, Translation2d& translation) {
  translation = Translation2d{units::meter_t{json.at("x").get<double>()},
                              units::meter_t{json.at("y").get<double>()}};
}
