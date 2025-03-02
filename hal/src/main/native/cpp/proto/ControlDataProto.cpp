// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "hal/proto/ControlData.h"

static_assert(sizeof(mrc::ControlFlags) == sizeof(uint32_t));

namespace {
constexpr uint32_t EnabledMask = 0x1;
constexpr uint32_t EStopMask = 0x8;
constexpr uint32_t FmsConnectedMask = 0x10;
constexpr uint32_t DsConnectedMask = 0x20;
constexpr uint32_t WatchdogActiveMask = 0x40;
constexpr uint32_t AllianceMask = 0x1F80;

constexpr uint32_t EnabledShift = 0;
constexpr uint32_t EStopShift = 3;
constexpr uint32_t FmsConnectedShift = 4;
constexpr uint32_t DsConnectedShift = 5;
constexpr uint32_t WatchdogActiveShift = 6;
constexpr uint32_t AllianceShift = 7;

#define WORD_TO_INT(Name) Ret |= (Word.Name << Name##Shift)

constexpr uint32_t FromControlWord(mrc::ControlFlags Word) {
  uint32_t Ret = 0;
  WORD_TO_INT(Enabled);
  WORD_TO_INT(EStop);
  WORD_TO_INT(FmsConnected);
  WORD_TO_INT(DsConnected);
  WORD_TO_INT(WatchdogActive);
  WORD_TO_INT(Alliance);
  return Ret;
}

#undef WORD_TO_INT

#define INT_TO_WORD(Name) Ret.Name = ((Word & Name##Mask) >> Name##Shift)

constexpr mrc::ControlFlags ToControlWord(uint32_t Word) {
  mrc::ControlFlags Ret = {};
  INT_TO_WORD(Enabled);
  INT_TO_WORD(EStop);
  INT_TO_WORD(FmsConnected);
  INT_TO_WORD(DsConnected);
  INT_TO_WORD(WatchdogActive);
  INT_TO_WORD(Alliance);
  return Ret;
}

}  // namespace

std::optional<mrc::ControlData> wpi::Protobuf<mrc::ControlData>::Unpack(
    InputStream& Stream) {
  wpi::UnpackCallback<mrc::Joystick, MRC_MAX_NUM_JOYSTICKS> JoystickCb;
  wpi::UnpackCallback<std::string> CurrentOpModeCb;
  wpi::UnpackCallback<std::string> SelectedTeleopOpModeCb;
  wpi::UnpackCallback<std::string> SelectedAutoOpModeCb;

  mrc_proto_ProtobufControlData Msg{
      .ControlWord = 0,
      .MatchTime = 0,
      .Joysticks = JoystickCb.Callback(),
      .CurrentOpMode = CurrentOpModeCb.Callback(),
      .SelectedTeleopOpMode = SelectedTeleopOpModeCb.Callback(),
      .SelectedAutoOpMode = SelectedTeleopOpModeCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Joysticks = JoystickCb.Items();
  auto CurrentOpMode = CurrentOpModeCb.Items();
  auto SelectedTeleopOpMode = SelectedTeleopOpModeCb.Items();
  auto SelectedAutoOpMode = SelectedAutoOpModeCb.Items();

  mrc::ControlData ControlData;

  if (!CurrentOpMode.empty()) {
    ControlData.MoveCurrentOpMode(std::move(CurrentOpMode[0]));
  }

  if (!SelectedTeleopOpMode.empty()) {
    ControlData.MoveSelectedTeleopOpMode(std::move(SelectedTeleopOpMode[0]));
  }

  if (!SelectedAutoOpMode.empty()) {
    ControlData.MoveSelectedAutoOpMode(std::move(SelectedAutoOpMode[0]));
  }

  ControlData.ControlWord = ToControlWord(Msg.ControlWord);
  ControlData.MatchTime = Msg.MatchTime;
  ControlData.SetJoystickCount(Joysticks.size());

  for (size_t i = 0; i < ControlData.GetJoystickCount(); i++) {
    ControlData.Joysticks()[i] = Joysticks[i];
  }

  return ControlData;
}

bool wpi::Protobuf<mrc::ControlData>::Pack(OutputStream& Stream,
                                           const mrc::ControlData& Value) {
  std::string_view CurrentOpMode = Value.GetCurrentOpMode();
  wpi::PackCallback CurrentOpModeCb{&CurrentOpMode};
  std::string_view SelectedTeleopOpMode = Value.GetSelectedTeleopOpMode();
  wpi::PackCallback SelectedTeleopOpModeCb{&SelectedTeleopOpMode};
  std::string_view SelectedAutoOpMode = Value.GetSelectedAutoOpMode();
  wpi::PackCallback SelectedAutoOpModeCb{&SelectedAutoOpMode};
  std::span<const mrc::Joystick> Sticks = Value.Joysticks();
  wpi::PackCallback Joysticks{Sticks};

  mrc_proto_ProtobufControlData Msg{
      .ControlWord = FromControlWord(Value.ControlWord),
      .MatchTime = Value.MatchTime,
      .Joysticks = Joysticks.Callback(),
      .CurrentOpMode = CurrentOpModeCb.Callback(),
      .SelectedTeleopOpMode = SelectedTeleopOpModeCb.Callback(),
      .SelectedAutoOpMode = SelectedAutoOpModeCb.Callback(),
  };

  return Stream.Encode(Msg);
}

std::optional<mrc::Joystick> wpi::Protobuf<mrc::Joystick>::Unpack(
    InputStream& Stream) {
  wpi::UnpackCallback<float, MRC_MAX_NUM_AXES> AxesCb;
  wpi::UnpackCallback<int16_t, MRC_MAX_NUM_POVS> PovsCb;

  mrc_proto_ProtobufJoystickData Msg{
      .ButtonCount = 0,
      .Buttons = 0,
      .Axes = AxesCb.Callback(),
      .POVs = PovsCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Axes = AxesCb.Items();
  auto Povs = PovsCb.Items();

  mrc::Joystick Joystick;
  Joystick.Axes.SetCount(Axes.size());

  for (size_t i = 0; i < Joystick.Axes.GetCount(); i++) {
    Joystick.Axes.Axes()[i] = Axes[i];
  }

  Joystick.Povs.SetCount(Povs.size());

  for (size_t i = 0; i < Joystick.Povs.GetCount(); i++) {
    Joystick.Povs.Povs()[i] = Povs[i];
  }

  Joystick.Buttons.SetCount(Msg.ButtonCount);
  Joystick.Buttons.Buttons = Msg.Buttons;

  return Joystick;
}

bool wpi::Protobuf<mrc::Joystick>::Pack(OutputStream& Stream,
                                        const mrc::Joystick& Value) {
  wpi::PackCallback AxesCb{Value.Axes.Axes()};
  wpi::PackCallback PovsCb{Value.Povs.Povs()};

  mrc_proto_ProtobufJoystickData Msg{
      .ButtonCount = static_cast<uint32_t>(Value.Buttons.GetCount()),
      .Buttons = Value.Buttons.Buttons,
      .Axes = AxesCb.Callback(),
      .POVs = PovsCb.Callback(),
  };

  return Stream.Encode(Msg);
}
