// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/servo/RotaryServo.h"

namespace frc {

/* Hitec HS-322HD servo provided in the FIRST Kit of Parts in 2008. */
class HS322HD : public RotaryServo {
 public:
  explicit HS322HD(int channel);

  HS322HD(HS322HD&&) = default;
  HS322HD& operator=(HS322HD&&) = default;
};

}  // namespace frc
