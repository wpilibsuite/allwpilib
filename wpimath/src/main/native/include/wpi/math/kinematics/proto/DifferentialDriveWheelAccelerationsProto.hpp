// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Protobuf<wpi::math::DifferentialDriveWheelAccelerations> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelAccelerations;
  using InputStream = wpi::util::ProtoInputStream<
      wpi::math::DifferentialDriveWheelAccelerations>;
  using OutputStream = wpi::util::ProtoOutputStream<
      wpi::math::DifferentialDriveWheelAccelerations>;
  static std::optional<wpi::math::DifferentialDriveWheelAccelerations> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveWheelAccelerations& value);
};
