#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

template <>
struct wpi::Protobuf<mrc::JoystickDescriptor> {
  using MessageStruct = mrc_proto_ProtobufJoystickDescriptor;
  using InputStream = wpi::ProtoInputStream<mrc::JoystickDescriptor>;
  using OutputStream = wpi::ProtoOutputStream<mrc::JoystickDescriptor>;
  static std::optional<mrc::JoystickDescriptor> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickDescriptor& Value);
};
