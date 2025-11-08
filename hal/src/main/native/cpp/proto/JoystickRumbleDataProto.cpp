// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/proto/JoystickRumbleData.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<mrc::JoystickRumbleData>
wpi::util::Protobuf<mrc::JoystickRumbleData>::Unpack(InputStream& Stream) {
  wpi::util::UnpackCallback<uint16_t, MRC_MAX_NUM_RUMBLE> RumbleCb;

  mrc_proto_ProtobufJoystickRumbleData Msg{
      .Value = RumbleCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Rumbles = RumbleCb.Items();

  mrc::JoystickRumbleData Rumble;
  Rumble.SetCount(Rumbles.size());

  for (size_t i = 0; i < Rumble.GetCount(); i++) {
    Rumble.Rumbles()[i] = Rumbles[i];
  }

  return Rumble;
}

bool wpi::util::Protobuf<mrc::JoystickRumbleData>::Pack(
    OutputStream& Stream, const mrc::JoystickRumbleData& Value) {
  wpi::util::PackCallback RumbleCb{Value.Rumbles()};

  mrc_proto_ProtobufJoystickRumbleData Msg{
      .Value = RumbleCb.Callback(),
  };

  return Stream.Encode(Msg);
}
