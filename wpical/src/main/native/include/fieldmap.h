// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>
#include <vector>

#include <tagpose.h>
#include <wpi/json.h>

class fieldmap {
 public:
  fieldmap();
  explicit fieldmap(wpi::json map);
  tag::pose getTag(int tag);
  int getNumTags();
  double minimizeAngle(double angle);

 private:
  std::vector<tag::pose> tagVec;
};
