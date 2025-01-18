// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "hal/proto/JoystickOutputData.h"

std::optional<mrc::JoystickOutputData>
wpi::Protobuf<mrc::JoystickOutputData>::Unpack(InputStream& Stream) {
  mrc_proto_ProtobufJoystickOutputData Msg;

  if (!Stream.Decode(Msg)) {
    return {};
  }

  return mrc::JoystickOutputData{
      .HidOutputs = Msg.HidOutputs,
      .LeftRumble = Msg.LeftRumble,
      .RightRumble = Msg.RightRumble,
  };
}

bool wpi::Protobuf<mrc::JoystickOutputData>::Pack(
    OutputStream& Stream, const mrc::JoystickOutputData& Value) {
  mrc_proto_ProtobufJoystickOutputData Msg{
      .HidOutputs = Value.HidOutputs,
      .LeftRumble = Value.LeftRumble,
      .RightRumble = Value.RightRumble,
  };

  return Stream.Encode(Msg);
}
