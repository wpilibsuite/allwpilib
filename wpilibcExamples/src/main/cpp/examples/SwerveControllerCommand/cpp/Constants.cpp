// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Constants.h"

namespace AutoConstants {

const frc::TrapezoidProfile<units::radians>::Constraints
    THETA_CONTROLLER_CONSTRAINTS{MAX_ANGULAR_SPEED, MAX_ANGULAR_ACCELERATION};

}  // namespace AutoConstants
