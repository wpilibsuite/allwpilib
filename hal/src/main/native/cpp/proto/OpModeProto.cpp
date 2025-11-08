// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>
#include <vector>

#include "wpi/hal/proto/OpMode.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<mrc::OpMode> wpi::util::Protobuf<mrc::OpMode>::Unpack(
    InputStream& Stream) {
  wpi::util::UnpackCallback<std::string> NameCb;

  mrc_proto_ProtobufOpMode Msg;
  Msg.Name = NameCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Name = NameCb.Items();

  if (Name.empty()) {
    return {};
  }

  mrc::OpMode OutputData;
  OutputData.MoveName(std::move(Name[0]));

  OutputData.Hash = mrc::OpModeHash::FromValue(Msg.Hash);

  return OutputData;
}

bool wpi::util::Protobuf<mrc::OpMode>::Pack(OutputStream& Stream,
                                      const mrc::OpMode& Value) {
  std::string_view EventNameStr = Value.GetName();
  wpi::util::PackCallback EventName{&EventNameStr};

  mrc_proto_ProtobufOpMode Msg{
      .Hash = Value.Hash.ToValue(),
      .Name = EventName.Callback(),
  };

  return Stream.Encode(Msg);
}

std::optional<std::vector<mrc::OpMode>>
wpi::util::Protobuf<std::vector<mrc::OpMode>>::Unpack(InputStream& Stream) {
  wpi::util::StdVectorUnpackCallback<mrc::OpMode> ModesCb;
  ModesCb.SetLimits(DecodeLimits::Add);

  mrc_proto_ProtobufAvailableOpModes Msg;
  Msg.Modes = ModesCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  return ModesCb.Vec();
}

bool wpi::util::Protobuf<std::vector<mrc::OpMode>>::Pack(
    OutputStream& Stream, const std::vector<mrc::OpMode>& Value) {
  std::span<const mrc::OpMode> ModesSpan = Value;
  wpi::util::PackCallback Modes{ModesSpan};

  mrc_proto_ProtobufAvailableOpModes Msg{
      .Modes = Modes.Callback(),
  };

  return Stream.Encode(Msg);
}
