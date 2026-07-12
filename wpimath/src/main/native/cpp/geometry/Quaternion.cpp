// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Quaternion.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const Quaternion& quaternion) {
  json.set_object();
  json["W"] = quaternion.W();
  json["X"] = quaternion.X();
  json["Y"] = quaternion.Y();
  json["Z"] = quaternion.Z();
}

void wpi::math::from_json(const wpi::util::json& json, Quaternion& quaternion) {
  quaternion = Quaternion{json.at("W").get_number(), json.at("X").get_number(),
                          json.at("Y").get_number(), json.at("Z").get_number()};
}
