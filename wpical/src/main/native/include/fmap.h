// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include <Eigen/Core>
#include <frc/apriltag/AprilTagFieldLayout.h>
#include <wpi/json.h>

namespace fmap {
struct Fiducial {
  std::string family;
  int id;
  double size;
  Eigen::Matrix4d transform;
  int unique = 1;
};

class Fieldmap {
 public:
  Fieldmap() = default;
  Fieldmap(std::string type, std::vector<Fiducial> fiducials);

  explicit Fieldmap(const frc::AprilTagFieldLayout& layout);

  std::string type;
  std::vector<Fiducial> fiducials;
};

void to_json(wpi::json& json, const Fiducial& layout);

void from_json(const wpi::json& json, Fiducial& layout);

void to_json(wpi::json& json, const Fieldmap& layout);

void from_json(const wpi::json& json, Fieldmap& layout);
}  // namespace fmap
