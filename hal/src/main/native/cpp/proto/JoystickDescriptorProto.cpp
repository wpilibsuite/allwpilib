// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "hal/proto/JoystickDescriptor.h"

std::optional<mrc::JoystickDescriptor>
wpi::Protobuf<mrc::JoystickDescriptor>::Unpack(InputStream& Stream) {
  wpi::UnpackCallback<std::string> JoystickNameCb;
  wpi::UnpackCallback<uint8_t, MRC_MAX_NUM_AXES> AxisTypesCb;

  mrc_proto_ProtobufJoystickDescriptor Msg;
  Msg.JoystickName = JoystickNameCb.Callback();
  Msg.AxisTypes = AxisTypesCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto JoystickName = JoystickNameCb.Items();
  auto AxisTypes = AxisTypesCb.Items();

  if (JoystickName.empty()) {
    return {};
  }

  mrc::JoystickDescriptor OutputData;
  OutputData.MoveName(std::move(JoystickName[0]));

  OutputData.SetAxesCount(AxisTypes.size());

  for (size_t i = 0; i < OutputData.GetAxesCount(); i++) {
    OutputData.AxesTypes()[i] = AxisTypes[i];
  }

  OutputData.SetPovsCount(Msg.PovCount);
  OutputData.SetButtonsCount(Msg.ButtonCount);

  OutputData.IsGamepad = Msg.IsGamepad ? 1 : 0;
  OutputData.Type = Msg.JoystickType;
  OutputData.RumbleCount = Msg.RumbleCount;

  return OutputData;
}

bool wpi::Protobuf<mrc::JoystickDescriptor>::Pack(
    OutputStream& Stream, const mrc::JoystickDescriptor& Value) {
  std::string_view JoystickName = Value.GetName();
  wpi::PackCallback JoystickNameCb{&JoystickName};

  std::span<const uint8_t> AxisTypes = Value.AxesTypes();
  wpi::PackCallback AxisTypesCb{AxisTypes};

  mrc_proto_ProtobufJoystickDescriptor Msg{
      .JoystickName = JoystickNameCb.Callback(),
      .AxisTypes = AxisTypesCb.Callback(),
      .IsGamepad = Value.IsGamepad ? true : false,
      .JoystickType = Value.Type,
      .ButtonCount = static_cast<int32_t>(Value.GetButtonsCount()),
      .PovCount = static_cast<int32_t>(Value.GetPovsCount()),
      .RumbleCount = Value.RumbleCount,
  };

  return Stream.Encode(Msg);
}
