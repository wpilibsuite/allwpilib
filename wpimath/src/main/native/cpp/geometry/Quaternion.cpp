// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Quaternion.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const Quaternion& quaternion) {
  json = wpi::util::json{{"W", quaternion.W()},
                         {"X", quaternion.X()},
                         {"Y", quaternion.Y()},
                         {"Z", quaternion.Z()}};
}

void wpi::math::from_json(const wpi::util::json& json, Quaternion& quaternion) {
  quaternion =
      Quaternion{json.at("W").get<double>(), json.at("X").get<double>(),
                 json.at("Y").get<double>(), json.at("Z").get<double>()};
}
