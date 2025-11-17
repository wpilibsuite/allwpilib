// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include "wpi/hal/proto/ControlData.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

static_assert(sizeof(mrc::ControlFlags) == sizeof(uint32_t));

namespace {
constexpr uint32_t EnabledMask = 0x1;
constexpr uint32_t AutoMask = 0x2;
constexpr uint32_t TestMask = 0x4;
constexpr uint32_t EStopMask = 0x8;
constexpr uint32_t FmsConnectedMask = 0x10;
constexpr uint32_t DsConnectedMask = 0x20;
constexpr uint32_t WatchdogActiveMask = 0x40;
constexpr uint32_t AllianceMask = 0x1F80;

constexpr uint32_t EnabledShift = 0;
constexpr uint32_t AutoShift = 1;
constexpr uint32_t TestShift = 2;
constexpr uint32_t EStopShift = 3;
constexpr uint32_t FmsConnectedShift = 4;
constexpr uint32_t DsConnectedShift = 5;
constexpr uint32_t WatchdogActiveShift = 6;
constexpr uint32_t AllianceShift = 7;

#define WORD_TO_INT(Name) Ret |= (Word.Name << Name##Shift)

constexpr uint32_t FromControlWord(mrc::ControlFlags Word) {
  uint32_t Ret = 0;
  WORD_TO_INT(Enabled);
  WORD_TO_INT(Auto);
  WORD_TO_INT(Test);
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
  INT_TO_WORD(Auto);
  INT_TO_WORD(Test);
  INT_TO_WORD(EStop);
  INT_TO_WORD(FmsConnected);
  INT_TO_WORD(DsConnected);
  INT_TO_WORD(WatchdogActive);
  INT_TO_WORD(Alliance);
  return Ret;
}

}  // namespace

std::optional<mrc::ControlData> wpi::util::Protobuf<mrc::ControlData>::Unpack(
    InputStream& Stream) {
  wpi::util::UnpackCallback<mrc::Joystick, MRC_MAX_NUM_JOYSTICKS> JoystickCb;

  mrc_proto_ProtobufControlData Msg{
      .ControlWord = 0,
      .MatchTime = 0,
      .Joysticks = JoystickCb.Callback(),
      .CurrentOpMode = 0,
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Joysticks = JoystickCb.Items();

  mrc::ControlData ControlData;

  ControlData.ControlWord = ToControlWord(Msg.ControlWord);
  ControlData.MatchTime = Msg.MatchTime;
  ControlData.CurrentOpMode = mrc::OpModeHash::FromValue(Msg.CurrentOpMode);
  ControlData.SetJoystickCount(Joysticks.size());

  for (size_t i = 0; i < ControlData.GetJoystickCount(); i++) {
    ControlData.Joysticks()[i] = std::move(Joysticks[i]);
  }

  return ControlData;
}

bool wpi::util::Protobuf<mrc::ControlData>::Pack(
    OutputStream& Stream, const mrc::ControlData& Value) {
  std::span<const mrc::Joystick> Sticks = Value.Joysticks();
  wpi::util::PackCallback Joysticks{Sticks};

  mrc_proto_ProtobufControlData Msg{
      .ControlWord = FromControlWord(Value.ControlWord),
      .MatchTime = Value.MatchTime,
      .Joysticks = Joysticks.Callback(),
      .CurrentOpMode = Value.CurrentOpMode.ToValue(),
  };

  return Stream.Encode(Msg);
}

std::optional<mrc::Joystick> wpi::util::Protobuf<mrc::Joystick>::Unpack(
    InputStream& Stream) {
  wpi::util::UnpackCallback<int16_t, MRC_MAX_NUM_AXES> AxesCb;

  mrc_proto_ProtobufJoystickData Msg{
      .AvailableButtons = 0,
      .Buttons = 0,
      .AvailableAxes = 0,
      .Axes = AxesCb.Callback(),
      .POVCount = 0,
      .POVs = 0,
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Axes = AxesCb.Items();

  mrc::Joystick Joystick;
  Joystick.Axes.SetAvailable(Msg.AvailableAxes);
  auto JoystickAxesCount =
      (std::min)(Joystick.Axes.GetMaxAvailableCount(), Axes.size());

  for (size_t i = 0; i < JoystickAxesCount; i++) {
    Joystick.Axes.Axes()[i] = Axes[i];
  }

  Joystick.Buttons.SetAvailable(Msg.AvailableButtons);
  Joystick.Buttons.Buttons = Msg.Buttons;

  Joystick.Povs.SetCount(Msg.POVCount);
  uint32_t PovsStore = Msg.POVs;
  for (size_t i = 0; i < Joystick.Povs.GetCount(); i++) {
    uint8_t Val = PovsStore & 0xF;
    PovsStore >>= 4;
    Joystick.Povs.Povs()[i] = Val;
  }

  return Joystick;
}

bool wpi::util::Protobuf<mrc::Joystick>::Pack(OutputStream& Stream,
                                              const mrc::Joystick& Value) {
  wpi::util::PackCallback AxesCb{Value.Axes.Axes()};

  uint32_t PovsStore = 0;
  for (int i = static_cast<int>(Value.Povs.GetCount()) - 1; i >= 0; i--) {
    PovsStore <<= 4;
    PovsStore |= Value.Povs.Povs()[i] & 0xF;
  }

  mrc_proto_ProtobufJoystickData Msg{
      .AvailableButtons = Value.Buttons.GetAvailable(),
      .Buttons = Value.Buttons.Buttons,
      .AvailableAxes = Value.Axes.GetAvailable(),
      .Axes = AxesCb.Callback(),
      .POVCount = static_cast<uint32_t>(Value.Povs.GetCount()),
      .POVs = PovsStore,
  };

  return Stream.Encode(Msg);
}
