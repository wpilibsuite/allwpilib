// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <nlohmann/json.hpp>
#include <tagpose.h>

class fieldmap {
 public:
  fieldmap() = default;
  explicit fieldmap(nlohmann::json map);
  tag::pose getTag(int tag);
  int getNumTags();
  double minimizeAngle(double angle);

 private:
  std::vector<tag::pose> tagVec;
};
