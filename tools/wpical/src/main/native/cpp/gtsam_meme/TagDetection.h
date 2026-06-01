// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <utility>
#include <vector>

struct TargetCorner {
  double x;
  double y;
};

struct TagDetection {
  int32_t id;
  std::vector<TargetCorner> corners;

  inline int GetFiducialId() const { return id; }
};
