// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/FieldTag.hpp"

#include "wpi/util/json.hpp"

using namespace wpi::fields;

void wpi::fields::to_json(wpi::util::json& json, const FieldTag& tag) {
  json = wpi::util::json::object("ID", tag.ID, "pose", tag.pose);
}

void wpi::fields::from_json(const wpi::util::json& json, FieldTag& tag) {
  tag.ID = json.at("ID").get_int();
  tag.pose = json.at("pose").get<wpi::math::Pose3d>();
}
