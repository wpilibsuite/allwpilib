// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTag.h"

#include <wpi/json.h>

using namespace frc;

void frc::to_json(wpi::json& json, const AprilTag& apriltag) {
  json = wpi::json{{"ID", apriltag.ID}, {"pose", apriltag.pose}};
}

void frc::from_json(const wpi::json& json, AprilTag& apriltag) {
  apriltag.ID = json.at("ID").get<int>();
  apriltag.pose = json.at("pose").get<Pose3d>();
}
