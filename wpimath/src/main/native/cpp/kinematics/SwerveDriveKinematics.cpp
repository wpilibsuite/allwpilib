// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/SwerveDriveKinematics.h"

namespace frc {

template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<2>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<3>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<4>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<6>;

}  // namespace frc
