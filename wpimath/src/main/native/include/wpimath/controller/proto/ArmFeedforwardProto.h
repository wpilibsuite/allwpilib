// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/controller/ArmFeedforward.h"
#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::ArmFeedforward> {
  using MessageStruct = wpi_proto_ProtobufArmFeedforward;
  using InputStream = wpi::ProtoInputStream<wpimath::ArmFeedforward>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::ArmFeedforward>;
  static std::optional<wpimath::ArmFeedforward> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::ArmFeedforward& value);
};
