// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include "wpi/hal/proto/JoystickDescriptors.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<mrc::JoystickDescriptors>
wpi::util::Protobuf<mrc::JoystickDescriptors>::Unpack(InputStream& Stream) {
  wpi::util::UnpackCallback<mrc::JoystickDescriptor, MRC_MAX_NUM_JOYSTICKS>
      JoystickDescriptorsCb;

  mrc_proto_ProtobufJoystickDescriptors Msg;
  Msg.Descriptors = JoystickDescriptorsCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Descriptors = JoystickDescriptorsCb.Items();

  mrc::JoystickDescriptors OutputData;
  OutputData.SetDescriptorCount(Descriptors.size());
  for (size_t i = 0; i < OutputData.GetDescriptorCount(); i++) {
    OutputData.Descriptors()[i] = std::move(Descriptors[i]);
  }

  return OutputData;
}

bool wpi::util::Protobuf<mrc::JoystickDescriptors>::Pack(
    OutputStream& Stream, const mrc::JoystickDescriptors& Value) {
  std::span<const mrc::JoystickDescriptor> Descriptors = Value.Descriptors();
  wpi::util::PackCallback JoystickDescriptorsCb{Descriptors};

  mrc_proto_ProtobufJoystickDescriptors Msg{
      .Descriptors = JoystickDescriptorsCb.Callback(),
  };

  return Stream.Encode(Msg);
}

std::optional<mrc::JoystickDescriptor>
wpi::util::Protobuf<mrc::JoystickDescriptor>::Unpack(InputStream& Stream) {
  wpi::util::UnpackCallback<std::string> JoystickNameCb;

  mrc_proto_ProtobufJoystickDescriptor Msg;
  Msg.JoystickName = JoystickNameCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto JoystickName = JoystickNameCb.Items();

  mrc::JoystickDescriptor OutputData;
  if (!JoystickName.empty()) {
    OutputData.MoveName(std::move(JoystickName[0]));
  }

  OutputData.IsGamepad = Msg.IsGamepad ? 1 : 0;
  OutputData.GamepadType = Msg.GamepadType;
  OutputData.SupportedOutputs = Msg.SupportedOutputs;

  return OutputData;
}

bool wpi::util::Protobuf<mrc::JoystickDescriptor>::Pack(
    OutputStream& Stream, const mrc::JoystickDescriptor& Value) {
  std::string_view JoystickName = Value.GetName();
  wpi::util::PackCallback JoystickNameCb{&JoystickName};

  mrc_proto_ProtobufJoystickDescriptor Msg{
      .JoystickName = JoystickNameCb.Callback(),
      .IsGamepad = Value.IsGamepad ? true : false,
      .GamepadType = Value.GamepadType,
      .SupportedOutputs = Value.SupportedOutputs,
  };

  return Stream.Encode(Msg);
}
