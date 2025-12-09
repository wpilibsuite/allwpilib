// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct wpi::util::Protobuf<mrc::JoystickOutput> {
  using MessageStruct = mrc_proto_ProtobufJoystickOutput;
  using InputStream = wpi::util::ProtoInputStream<mrc::JoystickOutput>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::JoystickOutput>;
  static std::optional<mrc::JoystickOutput> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickOutput& Value);
};

template <>
struct wpi::util::Protobuf<mrc::JoystickOutputs> {
  using MessageStruct = mrc_proto_ProtobufJoystickOutputs;
  using InputStream = wpi::util::ProtoInputStream<mrc::JoystickOutputs>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::JoystickOutputs>;
  static std::optional<mrc::JoystickOutputs> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickOutputs& Value);
};
