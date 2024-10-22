// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Rotation3d.h"

#include <wpi/json.h>

void frc::to_json(wpi::json& json, const Rotation3d& rotation) {
  json = wpi::json{{"quaternion", rotation.GetQuaternion()}};
}

void frc::from_json(const wpi::json& json, Rotation3d& rotation) {
  rotation = Rotation3d{json.at("quaternion").get<Quaternion>()};
}
