// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Constants.h"

using namespace DriveConstants;

const frc::DifferentialDriveKinematics DriveConstants::kDriveKinematics(
    kTrackwidth);

const frc::LinearSystem<2, 2, 2> DriveConstants::kDrivetrainPlant =
    frc::LinearSystemId::IdentifyDrivetrainSystem(kv, ka, kvAngular, kaAngular);
