// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/Quaternion.h"

#include <wpi/json.h>

void wpimath::to_json(wpi::json& json, const Quaternion& quaternion) {
  json = wpi::json{{"W", quaternion.W()},
                   {"X", quaternion.X()},
                   {"Y", quaternion.Y()},
                   {"Z", quaternion.Z()}};
}

void wpimath::from_json(const wpi::json& json, Quaternion& quaternion) {
  quaternion =
      Quaternion{json.at("W").get<double>(), json.at("X").get<double>(),
                 json.at("Y").get<double>(), json.at("Z").get<double>()};
}
