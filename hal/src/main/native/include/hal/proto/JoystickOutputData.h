#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

template <>
struct wpi::Protobuf<mrc::JoystickOutputData> {
  using MessageStruct = mrc_proto_ProtobufJoystickOutputData;
  using InputStream = wpi::ProtoInputStream<mrc::JoystickOutputData>;
  using OutputStream = wpi::ProtoOutputStream<mrc::JoystickOutputData>;
  static std::optional<mrc::JoystickOutputData> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickOutputData& Value);
};
