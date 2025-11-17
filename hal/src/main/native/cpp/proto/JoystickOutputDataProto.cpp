// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include "wpi/hal/proto/JoystickOutput.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<mrc::JoystickOutputs>
wpi::util::Protobuf<mrc::JoystickOutputs>::Unpack(InputStream& Stream) {
  wpi::util::UnpackCallback<mrc::JoystickOutput, MRC_MAX_NUM_JOYSTICKS>
      JoystickOutputsCb;

  mrc_proto_ProtobufJoystickOutputs Msg;
  Msg.Outputs = JoystickOutputsCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Outputs = JoystickOutputsCb.Items();

  mrc::JoystickOutputs OutputData;
  OutputData.SetOutputCount(Outputs.size());
  for (size_t i = 0; i < OutputData.GetOutputCount(); i++) {
    OutputData.Outputs()[i] = std::move(Outputs[i]);
  }

  return OutputData;
}

bool wpi::util::Protobuf<mrc::JoystickOutputs>::Pack(
    OutputStream& Stream, const mrc::JoystickOutputs& Value) {
  std::span<const mrc::JoystickOutput> Outputs = Value.Outputs();
  wpi::util::PackCallback JoystickOutputsCb{Outputs};

  mrc_proto_ProtobufJoystickOutputs Msg{
      .Outputs = JoystickOutputsCb.Callback(),
  };

  return Stream.Encode(Msg);
}

std::optional<mrc::JoystickOutput>
wpi::util::Protobuf<mrc::JoystickOutput>::Unpack(InputStream& Stream) {
  mrc_proto_ProtobufJoystickOutput Msg;

  if (!Stream.Decode(Msg)) {
    return {};
  }

  return mrc::JoystickOutput{
      .R = static_cast<uint8_t>((Msg.LEDs >> 16) & 0xFF),
      .G = static_cast<uint8_t>((Msg.LEDs >> 8) & 0xFF),
      .B = static_cast<uint8_t>(Msg.LEDs & 0xFF),
      .LeftRumble = static_cast<uint16_t>((Msg.Rumble >> 16) & 0xFFFF),
      .RightRumble = static_cast<uint16_t>(Msg.Rumble & 0xFFFF),
      .LeftTriggerRumble =
          static_cast<uint16_t>((Msg.TriggerRumble >> 16) & 0xFFFF),
      .RightTriggerRumble = static_cast<uint16_t>(Msg.TriggerRumble & 0xFFFF),
  };
}

bool wpi::util::Protobuf<mrc::JoystickOutput>::Pack(
    OutputStream& Stream, const mrc::JoystickOutput& Value) {
  mrc_proto_ProtobufJoystickOutput Msg{
      .LEDs = (static_cast<uint32_t>(Value.R) << 16) |
              (static_cast<uint32_t>(Value.G) << 8) |
              static_cast<uint32_t>(Value.B),
      .Rumble = (static_cast<uint32_t>(Value.LeftRumble) << 16) |
                static_cast<uint32_t>(Value.RightRumble),
      .TriggerRumble = (static_cast<uint32_t>(Value.LeftTriggerRumble) << 16) |
                       static_cast<uint32_t>(Value.RightTriggerRumble),
  };

  return Stream.Encode(Msg);
}
