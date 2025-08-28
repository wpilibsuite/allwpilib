// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpi/math/controller/ArmFeedforward.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::ArmFeedforward> {
  using MessageStruct = wpi_proto_ProtobufArmFeedforward;
  using InputStream = wpi::ProtoInputStream<wpi::math::ArmFeedforward>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::ArmFeedforward>;
  static std::optional<wpi::math::ArmFeedforward> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::ArmFeedforward& value);
};
