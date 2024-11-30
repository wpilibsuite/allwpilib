// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/protobuf/Protobuf.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"

template <>
struct wpi::Protobuf<mrc::JoystickDescriptor> {
  using MessageStruct = mrc_proto_ProtobufJoystickDescriptor;
  using InputStream = wpi::ProtoInputStream<mrc::JoystickDescriptor>;
  using OutputStream = wpi::ProtoOutputStream<mrc::JoystickDescriptor>;
  static std::optional<mrc::JoystickDescriptor> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickDescriptor& Value);
};
