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
  wpi::util::UnpackCallback<std::string> GroupCb;
  wpi::util::UnpackCallback<std::string> DescriptionCb;

  mrc_proto_ProtobufOpMode Msg;
  Msg.Name = NameCb.Callback();
  Msg.Group = GroupCb.Callback();
  Msg.Description = DescriptionCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Name = NameCb.Items();
  auto Group = GroupCb.Items();
  auto Description = DescriptionCb.Items();

  if (Name.empty() || Group.empty() || Description.empty()) {
    return {};
  }

  return mrc::OpMode{
      mrc::OpModeHash::FromValue(Msg.Hash),
      std::move(Name[0]),
      std::move(Group[0]),
      std::move(Description[0]),
      Msg.TextColor,
      Msg.BackgroundColor,
  };
}

bool wpi::util::Protobuf<mrc::OpMode>::Pack(OutputStream& Stream,
                                            const mrc::OpMode& Value) {
  std::string_view EventNameStr = Value.GetName();
  wpi::util::PackCallback EventName{&EventNameStr};
  std::string_view EventGroupStr = Value.GetGroup();
  wpi::util::PackCallback EventGroup{&EventGroupStr};
  std::string_view EventDescriptionStr = Value.GetDescription();
  wpi::util::PackCallback EventDescription{&EventDescriptionStr};

  mrc_proto_ProtobufOpMode Msg{
      .Hash = Value.Hash.ToValue(),
      .Name = EventName.Callback(),
      .Group = EventGroup.Callback(),
      .Description = EventDescription.Callback(),
      .TextColor = Value.GetTextColor(),
      .BackgroundColor = Value.GetBackgroundColor(),
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
