// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/DifferentialDriveWheelPositions.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <>
struct WPILIB_DLLEXPORT
    wpi::Protobuf<wpi::math::DifferentialDriveWheelPositions> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelPositions;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::DifferentialDriveWheelPositions>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::DifferentialDriveWheelPositions>;
  static std::optional<wpi::math::DifferentialDriveWheelPositions> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveWheelPositions& value);
};
