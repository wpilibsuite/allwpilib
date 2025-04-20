// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/ExponentialPIDController.h"

int frc::detail::IncrementAndGetExponentialPIDControllerInstances() {
  static int instances = 0;
  return ++instances;
}
