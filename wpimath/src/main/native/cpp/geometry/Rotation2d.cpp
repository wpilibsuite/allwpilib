// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Rotation2d.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const Rotation2d& rotation) {
  json = wpi::util::json{{"radians", rotation.Radians().value()}};
}

void wpi::math::from_json(const wpi::util::json& json, Rotation2d& rotation) {
  rotation = Rotation2d{wpi::units::radian_t{json.at("radians").get<double>()}};
}
