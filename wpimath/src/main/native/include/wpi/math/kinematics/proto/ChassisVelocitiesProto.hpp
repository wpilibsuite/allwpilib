// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::ChassisVelocities> {
  using MessageStruct = wpi_proto_ProtobufChassisVelocities;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::ChassisVelocities>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::ChassisVelocities>;
  static std::optional<wpi::math::ChassisVelocities> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::ChassisVelocities& value);
};
