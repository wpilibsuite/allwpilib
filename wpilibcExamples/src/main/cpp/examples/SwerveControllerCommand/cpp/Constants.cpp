// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Constants.h"

namespace AutoConstants {

const frc::TrapezoidProfile<units::radians>::Constraints
    kThetaControllerConstraints{kMaxAngularSpeed, kMaxAngularAcceleration};

}  // namespace AutoConstants
