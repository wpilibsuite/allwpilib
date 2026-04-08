// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Rotation3d.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const Rotation3d& rotation) {
  json = wpi::util::json{{"quaternion", rotation.GetQuaternion()}};
}

void wpi::math::from_json(const wpi::util::json& json, Rotation3d& rotation) {
  rotation = Rotation3d{json.at("quaternion").get<Quaternion>()};
}
