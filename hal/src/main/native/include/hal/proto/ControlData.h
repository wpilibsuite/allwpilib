#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

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
