// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/ChassisAccelerations.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::ChassisAccelerations> {
  using MessageStruct = wpi_proto_ProtobufChassisAccelerations;
  using InputStream = wpi::ProtoInputStream<frc::ChassisAccelerations>;
  using OutputStream = wpi::ProtoOutputStream<frc::ChassisAccelerations>;
  static std::optional<frc::ChassisAccelerations> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::ChassisAccelerations& value);
};
