// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/json.h>
#include <wpi/math/geometry/Rotation2d.h>

void wpi::math::to_json(wpi::json& json, const Rotation2d& rotation) {
  json = wpi::json{{"radians", rotation.Radians().value()}};
}

void wpi::math::from_json(const wpi::json& json, Rotation2d& rotation) {
  rotation = Rotation2d{units::radian_t{json.at("radians").get<double>()}};
}
