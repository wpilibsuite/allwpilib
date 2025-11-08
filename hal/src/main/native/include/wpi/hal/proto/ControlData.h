// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct wpi::Protobuf<mrc::ControlData> {
  using MessageStruct = mrc_proto_ProtobufControlData;
  using InputStream = wpi::ProtoInputStream<mrc::ControlData>;
  using OutputStream = wpi::ProtoOutputStream<mrc::ControlData>;
  static std::optional<mrc::ControlData> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::ControlData& Value);
};

template <>
struct wpi::Protobuf<mrc::Joystick> {
  using MessageStruct = mrc_proto_ProtobufJoystickData;
  using InputStream = wpi::ProtoInputStream<mrc::Joystick>;
  using OutputStream = wpi::ProtoOutputStream<mrc::Joystick>;
  static std::optional<mrc::Joystick> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::Joystick& Value);
};
