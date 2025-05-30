// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <bit>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "mrc/NtNetComm.h"

namespace mrc {

struct OpModeHash {
  uint64_t Hash : 56 = 0;
  uint64_t IsAuto : 1 = 0;
  uint64_t IsTest : 1 = 0;
  uint64_t IsEnabled : 1 = 0;
  uint64_t Reserved : 5 = 0;

  static constexpr uint64_t AutoMask = 0x0100000000000000;
  static constexpr uint64_t TestMask = 0x0200000000000000;
  static constexpr uint64_t EnabledMask = 0x0400000000000000;
  static constexpr uint64_t HashMask = 0x00FFFFFFFFFFFFFF;

  constexpr static OpModeHash MakeTest(uint64_t Hash, bool Enabled = false) {
    OpModeHash FullHash;
    FullHash.Hash = Hash & HashMask;
    FullHash.IsTest = 1;
    FullHash.IsEnabled = Enabled ? 1 : 0;
    return FullHash;
  }

  constexpr static OpModeHash MakeTele(uint64_t Hash, bool Enabled = false) {
    OpModeHash FullHash;
    FullHash.Hash = Hash & HashMask;
    FullHash.IsEnabled = Enabled ? 1 : 0;
    return FullHash;
  }

  constexpr static OpModeHash MakeAuto(uint64_t Hash, bool Enabled = false) {
    OpModeHash FullHash;
    FullHash.Hash = Hash & HashMask;
    FullHash.IsAuto = 1;
    FullHash.IsEnabled = Enabled ? 1 : 0;
    return FullHash;
  }

  constexpr static OpModeHash FromValue(uint64_t Value) {
    OpModeHash RetVal;
    RetVal.Hash = Value & HashMask;
    RetVal.IsAuto = (Value & AutoMask) != 0;
    RetVal.IsTest = (Value & TestMask) != 0;
    RetVal.IsEnabled = (Value & EnabledMask) != 0;
    return RetVal;
  }

  constexpr uint64_t ToValue() const {
    uint64_t RetVal = Hash & HashMask;
    RetVal |= IsAuto ? AutoMask : 0;
    RetVal |= IsTest ? TestMask : 0;
    RetVal |= IsEnabled ? EnabledMask : 0;
    return RetVal;
  }
};

struct ControlFlags {
  uint32_t Enabled : 1 = 0;
  uint32_t Auto : 1 = 0;
  uint32_t Test : 1 = 0;
  uint32_t EStop : 1 = 0;
  uint32_t FmsConnected : 1 = 0;
  uint32_t DsConnected : 1 = 0;
  uint32_t WatchdogActive : 1 = 0;
  uint32_t Alliance : 6 = 0;
  uint32_t Reserved : 19 = 0;
};

struct JoystickAxes {
 public:
  std::span<int16_t> Axes() {
    return std::span{AxesStore.data(), GetMaxAvailableCount()};
  }

  std::span<const int16_t> Axes() const {
    return std::span{AxesStore.data(), GetMaxAvailableCount()};
  }

  void SetAvailable(uint16_t Available) {
    AvailableAxes = Available & ((1 << MRC_MAX_NUM_AXES) - 1);
    Count = static_cast<uint8_t>(16 - std::countl_zero(AvailableAxes));
  }

  uint16_t GetAvailable() const { return AvailableAxes; }

  void SetMaxAvailableCount(size_t _Count) {
    if (_Count > MRC_MAX_NUM_AXES) {
      _Count = MRC_MAX_NUM_AXES;
    }
    AvailableAxes = (1 << _Count) - 1;
    Count = static_cast<uint8_t>(_Count);
  }

  size_t GetMaxAvailableCount() const { return Count; }

 private:
  std::array<int16_t, MRC_MAX_NUM_AXES> AxesStore{};
  uint16_t AvailableAxes{0};
  uint8_t Count{0};
};

struct JoystickPovs {
 public:
  std::span<uint8_t> Povs() { return std::span{PovsStore.data(), GetCount()}; }

  std::span<const uint8_t> Povs() const {
    return std::span{PovsStore.data(), GetCount()};
  }

  void SetCount(uint8_t NewCount) {
    Count = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_POVS));
  }

  size_t GetCount() const { return Count; }

 private:
  std::array<uint8_t, MRC_MAX_NUM_POVS> PovsStore{};
  uint8_t Count{0};
};

struct JoystickButtons {
  uint64_t Buttons{0};

  void SetAvailable(uint64_t Available) {
    AvailableButtons = Available;
    Count = static_cast<uint8_t>(64 - std::countl_zero(Available));
  }

  uint64_t GetAvailable() const { return AvailableButtons; }

  void SetMaxAvailableCount(size_t _Count) {
    if (_Count > 63) {
      AvailableButtons = (std::numeric_limits<uint64_t>::max)();
      Count = MRC_MAX_NUM_BUTTONS;
      return;
    }
    AvailableButtons = (1ULL << _Count) - 1;
    Count = static_cast<uint8_t>(_Count);
  }

  size_t GetMaxAvailableCount() const { return Count; }

 private:
  uint64_t AvailableButtons{0};
  uint8_t Count{0};
};

struct Joystick {
  JoystickAxes Axes;
  JoystickPovs Povs;
  JoystickButtons Buttons;
};

struct ControlData {
  ControlFlags ControlWord;
  uint16_t MatchTime{0};
  OpModeHash CurrentOpMode;

  std::span<Joystick> Joysticks() {
    return std::span{JoysticksStore.data(), GetJoystickCount()};
  }

  std::span<const Joystick> Joysticks() const {
    return std::span{JoysticksStore.data(), GetJoystickCount()};
  }

  size_t GetJoystickCount() const { return JoystickCount; }

  void SetJoystickCount(uint8_t NewCount) {
    JoystickCount =
        (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_JOYSTICKS));
  }

 private:
  std::array<Joystick, MRC_MAX_NUM_JOYSTICKS> JoysticksStore;
  uint8_t JoystickCount{0};
};

struct JoystickRumbleData {
 public:
  std::span<uint16_t> Rumbles() {
    return std::span{RumbleStore.data(), GetCount()};
  }

  std::span<const uint16_t> Rumbles() const {
    return std::span{RumbleStore.data(), GetCount()};
  }

  void SetCount(uint8_t NewCount) {
    Count = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_RUMBLE));
  }

  size_t GetCount() const { return Count; }

 private:
  std::array<uint16_t, MRC_MAX_NUM_RUMBLE> RumbleStore;
  uint8_t Count{0};
};

enum class MatchType : uint8_t {
  None = 0,
  Practice = 1,
  Qualification = 2,
  Playoff = 3,
  Test = 4,
};

struct MatchInfo {
  uint16_t MatchNumber{0};
  uint8_t ReplayNumber{0};
  MatchType Type{MatchType::None};

  void SetEventName(std::string_view Name) {
    if (Name.size() > MRC_MAX_EVENT_NAME_LEN) {
      Name = Name.substr(0, MRC_MAX_EVENT_NAME_LEN);
    }
    EventName = Name;
  }

  void MoveEventName(std::string&& Name) {
    EventName = std::move(Name);
    if (EventName.size() > MRC_MAX_EVENT_NAME_LEN) {
      EventName.resize(MRC_MAX_EVENT_NAME_LEN);
    }
  }

  std::string_view GetEventName() const { return EventName; }

  std::span<uint8_t> WritableNameBuffer(size_t Len) {
    if (Len > MRC_MAX_EVENT_NAME_LEN) {
      Len = MRC_MAX_EVENT_NAME_LEN;
    }
    EventName.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(EventName.data()),
                              EventName.size()};
  }

  bool operator==(const MatchInfo& Other) const {
    return MatchNumber == Other.MatchNumber &&
           ReplayNumber == Other.ReplayNumber && Type == Other.Type &&
           EventName == Other.EventName;
  }

 private:
  std::string EventName;
};

struct JoystickDescriptor {
 public:
  bool IsGamepad{0};
  uint8_t Type{0};
  uint8_t RumbleCount{0};

  std::span<uint8_t> AxesTypes() {
    return std::span{AxesTypesStore.data(), GetAxesCount()};
  }

  std::span<const uint8_t> AxesTypes() const {
    return std::span{AxesTypesStore.data(), GetAxesCount()};
  }

  void SetAxesCount(uint8_t NewCount) {
    AxesCount = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_AXES));
  }

  size_t GetAxesCount() const { return AxesCount; }

  void SetPovsCount(uint8_t NewCount) {
    PovCount = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_POVS));
  }

  size_t GetPovsCount() const { return PovCount; }

  void SetButtonsCount(uint8_t NewCount) {
    ButtonCount =
        (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_BUTTONS));
  }

  size_t GetButtonsCount() const { return ButtonCount; }

  void SetName(std::string_view Name) {
    if (Name.size() > MRC_MAX_JOYSTICK_NAME_LEN) {
      Name = Name.substr(0, MRC_MAX_JOYSTICK_NAME_LEN);
    }
    JoystickName = Name;
  }

  void MoveName(std::string&& Name) {
    JoystickName = std::move(Name);
    if (JoystickName.size() > MRC_MAX_JOYSTICK_NAME_LEN) {
      JoystickName.resize(MRC_MAX_JOYSTICK_NAME_LEN);
    }
  }

  std::string_view GetName() const { return JoystickName; }

  std::span<uint8_t> WritableNameBuffer(size_t Len) {
    if (Len > MRC_MAX_JOYSTICK_NAME_LEN) {
      Len = MRC_MAX_JOYSTICK_NAME_LEN;
    }
    JoystickName.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(JoystickName.data()),
                              JoystickName.size()};
  }

 private:
  std::string JoystickName;
  std::array<uint8_t, MRC_MAX_NUM_AXES> AxesTypesStore;
  uint8_t AxesCount{0};
  uint8_t ButtonCount{0};
  uint8_t PovCount{0};
};

struct ErrorInfo {
  bool IsError{false};
  int32_t ErrorCode{0};

  void SetDetails(std::string_view NewDetails) {
    if (NewDetails.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      NewDetails = NewDetails.substr(0, MRC_MAX_ERROR_INFO_STR_LEN);
    }
    Details = NewDetails;
  }

  void MoveDetails(std::string&& NewDetails) {
    Details = std::move(NewDetails);
    if (Details.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      Details.resize(MRC_MAX_ERROR_INFO_STR_LEN);
    }
  }

  std::string_view GetDetails() const { return Details; }

  std::span<uint8_t> WritableDetailsBuffer(size_t Len) {
    if (Len > MRC_MAX_ERROR_INFO_STR_LEN) {
      Len = MRC_MAX_ERROR_INFO_STR_LEN;
    }
    Details.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(Details.data()),
                              Details.size()};
  }

  void SetLocation(std::string_view NewLocation) {
    if (NewLocation.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      NewLocation = NewLocation.substr(0, MRC_MAX_ERROR_INFO_STR_LEN);
    }
    Location = NewLocation;
  }

  void MoveLocation(std::string&& NewLocation) {
    Location = std::move(NewLocation);
    if (Location.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      Location.resize(MRC_MAX_ERROR_INFO_STR_LEN);
    }
  }

  std::string_view GetLocation() const { return Location; }

  std::span<uint8_t> WritableLocationBuffer(size_t Len) {
    if (Len > MRC_MAX_ERROR_INFO_STR_LEN) {
      Len = MRC_MAX_ERROR_INFO_STR_LEN;
    }
    Location.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(Location.data()),
                              Location.size()};
  }

  void SetCallStack(std::string_view NewCallStack) {
    if (NewCallStack.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      NewCallStack = NewCallStack.substr(0, MRC_MAX_ERROR_INFO_STR_LEN);
    }
    CallStack = NewCallStack;
  }

  void MoveCallStack(std::string&& NewCallStack) {
    CallStack = std::move(NewCallStack);
    if (CallStack.size() > MRC_MAX_ERROR_INFO_STR_LEN) {
      CallStack.resize(MRC_MAX_ERROR_INFO_STR_LEN);
    }
  }

  std::string_view GetCallStack() const { return CallStack; }

  std::span<uint8_t> WritableCallStackBuffer(size_t Len) {
    if (Len > MRC_MAX_ERROR_INFO_STR_LEN) {
      Len = MRC_MAX_ERROR_INFO_STR_LEN;
    }
    CallStack.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(CallStack.data()),
                              CallStack.size()};
  }

 private:
  std::string Details;
  std::string Location;
  std::string CallStack;
};

struct OpMode {
  OpMode(std::string_view _Name, OpModeHash _Hash) : Hash(_Hash) {
    SetName(_Name);
  }

  OpMode() = default;

  OpModeHash Hash;

  void SetName(std::string_view NewName) {
    if (NewName.size() > MRC_MAX_OPMODE_LEN) {
      NewName = NewName.substr(0, MRC_MAX_OPMODE_LEN);
    }
    Name = NewName;
  }

  void MoveName(std::string&& NewName) {
    Name = std::move(NewName);
    if (Name.size() > MRC_MAX_OPMODE_LEN) {
      Name.resize(MRC_MAX_OPMODE_LEN);
    }
  }

  std::string_view GetName() const { return Name; }

  std::span<uint8_t> WritableNameBuffer(size_t Len) {
    if (Len > MRC_MAX_OPMODE_LEN) {
      Len = MRC_MAX_OPMODE_LEN;
    }
    Name.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(Name.data()),
                              Name.size()};
  }

 private:
  std::string Name;
};

}  // namespace mrc
