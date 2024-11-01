// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/ChassisSpeeds.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::ChassisSpeeds> {
  static const pb_msgdesc_t* Message();
  static std::optional<frc::ChassisSpeeds> Unpack(
      wpi::ProtoInputStream& stream);
  static bool Pack(wpi::ProtoOutputStream& stream,
                   const frc::ChassisSpeeds& value);
};
