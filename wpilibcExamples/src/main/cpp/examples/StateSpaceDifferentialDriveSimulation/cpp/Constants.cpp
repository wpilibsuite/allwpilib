/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Constants.h"

using namespace DriveConstants;

const frc::DifferentialDriveKinematics DriveConstants::kDriveKinematics(
    kTrackwidth);

const frc::LinearSystem<2, 2, 2> DriveConstants::kDrivetrainPlant =
    frc::LinearSystemId::IdentifyDrivetrainSystem(kv, ka, kvAngular, kaAngular);
