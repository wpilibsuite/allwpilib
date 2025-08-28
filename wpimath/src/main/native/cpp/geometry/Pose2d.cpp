// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/Pose2d.h"

#include <wpi/json.h>

void wpimath::to_json(wpi::json& json, const Pose2d& pose) {
  json = wpi::json{{"translation", pose.Translation()},
                   {"rotation", pose.Rotation()}};
}

void wpimath::from_json(const wpi::json& json, Pose2d& pose) {
  pose = Pose2d{json.at("translation").get<Translation2d>(),
                json.at("rotation").get<Rotation2d>()};
}
