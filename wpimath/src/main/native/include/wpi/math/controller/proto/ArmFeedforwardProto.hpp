// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/controller/ArmFeedforward.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::ArmFeedforward> {
  using MessageStruct = wpi_proto_ProtobufArmFeedforward;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::ArmFeedforward>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::ArmFeedforward>;
  static std::optional<wpi::math::ArmFeedforward> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::ArmFeedforward& value);
};
