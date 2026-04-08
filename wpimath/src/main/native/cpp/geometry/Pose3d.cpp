// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Pose3d.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const Pose3d& pose) {
  json = wpi::util::json{{"translation", pose.Translation()},
                         {"rotation", pose.Rotation()}};
}

void wpi::math::from_json(const wpi::util::json& json, Pose3d& pose) {
  pose = Pose3d{json.at("translation").get<Translation3d>(),
                json.at("rotation").get<Rotation3d>()};
}
