// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct wpi::util::Protobuf<mrc::ControlData> {
  using MessageStruct = mrc_proto_ProtobufControlData;
  using InputStream = wpi::util::ProtoInputStream<mrc::ControlData>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::ControlData>;
  static std::optional<mrc::ControlData> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::ControlData& Value);
};

template <>
struct wpi::util::Protobuf<mrc::Joystick> {
  using MessageStruct = mrc_proto_ProtobufJoystickData;
  using InputStream = wpi::util::ProtoInputStream<mrc::Joystick>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::Joystick>;
  static std::optional<mrc::Joystick> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::Joystick& Value);
};

template <>
struct wpi::util::Protobuf<mrc::TouchpadFinger> {
  using MessageStruct = mrc_proto_ProtobufFingerData;
  using InputStream = wpi::util::ProtoInputStream<mrc::TouchpadFinger>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::TouchpadFinger>;
  static std::optional<mrc::TouchpadFinger> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::TouchpadFinger& Value);
};

template <>
struct wpi::util::Protobuf<mrc::Touchpad> {
  using MessageStruct = mrc_proto_ProtobufTouchpadData;
  using InputStream = wpi::util::ProtoInputStream<mrc::Touchpad>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::Touchpad>;
  static std::optional<mrc::Touchpad> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::Touchpad& Value);
};
