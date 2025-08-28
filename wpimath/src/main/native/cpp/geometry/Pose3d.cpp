// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/Pose3d.h"

#include <wpi/json.h>

void wpimath::to_json(wpi::json& json, const Pose3d& pose) {
  json = wpi::json{{"translation", pose.Translation()},
                   {"rotation", pose.Rotation()}};
}

void wpimath::from_json(const wpi::json& json, Pose3d& pose) {
  pose = Pose3d{json.at("translation").get<Translation3d>(),
                json.at("rotation").get<Rotation3d>()};
}
