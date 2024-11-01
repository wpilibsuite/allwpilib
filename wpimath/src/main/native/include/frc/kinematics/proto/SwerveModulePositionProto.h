// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveModulePosition.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::SwerveModulePosition> {
  static const pb_msgdesc_t* Message();
  static std::optional<frc::SwerveModulePosition> Unpack(
      wpi::ProtoInputStream& stream);
  static bool Pack(wpi::ProtoOutputStream& stream,
                   const frc::SwerveModulePosition& value);
};
