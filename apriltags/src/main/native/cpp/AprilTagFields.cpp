// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagFields.h"

#include <wpi/json.h>

namespace frc {

// C++ generated from resource files
std::string_view GetResource_2022_rapidreact_json();

AprilTagFieldLayout LoadAprilTagLayoutField(AprilTagField field) {
  std::string_view fieldString;
  switch (field) {
    case AprilTagField::k2022RapidReact:
      fieldString = GetResource_2022_rapidreact_json();
      break;
    case AprilTagField::kNumFields:
      throw std::invalid_argument("Invalid Field");
  }

  wpi::json json = wpi::json::parse(fieldString);
  return json.get<AprilTagFieldLayout>();
}

}  // namespace frc
