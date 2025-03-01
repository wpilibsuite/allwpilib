// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "mrc/NtNetComm.h"

namespace mrc {

struct ControlFlags {
  uint32_t Enabled : 1;
  uint32_t ModeReserved : 2;
  uint32_t EStop : 1;
  uint32_t FmsConnected : 1;
  uint32_t DsConnected : 1;
  uint32_t WatchdogActive : 1;
  uint32_t Alliance : 6;
  uint32_t Reserved : 19;
};

struct JoystickAxes {
 public:
  std::span<float> Axes() { return std::span{AxesStore.data(), GetCount()}; }

  std::span<const float> Axes() const {
    return std::span{AxesStore.data(), GetCount()};
  }

  void SetCount(uint8_t NewCount) {
    Count = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_AXES));
  }

  size_t GetCount() const { return Count; }

 private:
  std::array<float, MRC_MAX_NUM_AXES> AxesStore;
  uint8_t Count{0};
};

struct JoystickPovs {
 public:
  std::span<int16_t> Povs() { return std::span{PovsStore.data(), GetCount()}; }

  std::span<const int16_t> Povs() const {
    return std::span{PovsStore.data(), GetCount()};
  }

  void SetCount(uint8_t NewCount) {
    Count = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_POVS));
  }

  size_t GetCount() const { return Count; }

 private:
  std::array<int16_t, MRC_MAX_NUM_POVS> PovsStore;
  uint8_t Count{0};
};

struct JoystickButtons {
  uint32_t Buttons;

  void SetCount(uint8_t NewCount) {
    Count = (std::min)(NewCount, static_cast<uint8_t>(MRC_MAX_NUM_BUTTONS));
  }

  size_t GetCount() const { return Count; }

 private:
  uint8_t Count{0};
};

struct Joystick {
  JoystickAxes Axes;
  JoystickPovs Povs;
  JoystickButtons Buttons;
};

struct ControlData {
  ControlFlags ControlWord;
  float MatchTime;

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

  void SetCurrentOpMode(std::string_view Mode) {
    if (Mode.size() > MRC_MAX_OPMODE_LEN) {
      Mode = Mode.substr(0, MRC_MAX_OPMODE_LEN);
    }
    CurrentOpMode = Mode;
  }

  void MoveCurrentOpMode(std::string&& Mode) {
    CurrentOpMode = std::move(Mode);
    if (CurrentOpMode.size() > MRC_MAX_OPMODE_LEN) {
      CurrentOpMode.resize(MRC_MAX_OPMODE_LEN);
    }
  }

  std::string_view GetCurrentOpMode() const { return CurrentOpMode; }

  std::span<uint8_t> WritableCurrentOpModeBuffer(size_t Len) {
    if (Len > MRC_MAX_OPMODE_LEN) {
      Len = MRC_MAX_OPMODE_LEN;
    }
    CurrentOpMode.resize(Len);
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(CurrentOpMode.data()),
                              CurrentOpMode.size()};
  }

  void SetSelectedAutonOpMode(std::string_view Mode) {
    if (Mode.size() > MRC_MAX_OPMODE_LEN) {
      Mode = Mode.substr(0, MRC_MAX_OPMODE_LEN);
    }
    SelectedAutonOpMode = Mode;
  }

  void MoveSelectedAutonOpMode(std::string&& Mode) {
    SelectedAutonOpMode = std::move(Mode);
    if (SelectedAutonOpMode.size() > MRC_MAX_OPMODE_LEN) {
      SelectedAutonOpMode.resize(MRC_MAX_OPMODE_LEN);
    }
  }

  std::string_view GetSelectedAutonOpMode() const {
    return SelectedAutonOpMode;
  }

  std::span<uint8_t> WritableSelectedAutonOpModeBuffer(size_t Len) {
    if (Len > MRC_MAX_OPMODE_LEN) {
      Len = MRC_MAX_OPMODE_LEN;
    }
    SelectedAutonOpMode.resize(Len);
    return std::span<uint8_t>{
        reinterpret_cast<uint8_t*>(SelectedAutonOpMode.data()),
        SelectedAutonOpMode.size()};
  }

  void SetSelectedTeleopOpMode(std::string_view Mode) {
    if (Mode.size() > MRC_MAX_OPMODE_LEN) {
      Mode = Mode.substr(0, MRC_MAX_OPMODE_LEN);
    }
    SelectedTeleopOpMode = Mode;
  }

  void MoveSelectedTeleopOpMode(std::string&& Mode) {
    SelectedTeleopOpMode = std::move(Mode);
    if (SelectedTeleopOpMode.size() > MRC_MAX_OPMODE_LEN) {
      SelectedTeleopOpMode.resize(MRC_MAX_OPMODE_LEN);
    }
  }

  std::string_view GetSelectedTeleopOpMode() const {
    return SelectedTeleopOpMode;
  }

  std::span<uint8_t> WritableSelectedTeleopOpModeBuffer(size_t Len) {
    if (Len > MRC_MAX_OPMODE_LEN) {
      Len = MRC_MAX_OPMODE_LEN;
    }
    SelectedTeleopOpMode.resize(Len);
    return std::span<uint8_t>{
        reinterpret_cast<uint8_t*>(SelectedTeleopOpMode.data()),
        SelectedTeleopOpMode.size()};
  }

 private:
  std::array<Joystick, MRC_MAX_NUM_JOYSTICKS> JoysticksStore;
  uint8_t JoystickCount{0};
  std::string CurrentOpMode;
  std::string SelectedTeleopOpMode;
  std::string SelectedAutonOpMode;
};

struct JoystickOutputData {
  uint32_t HidOutputs{0};
  float LeftRumble{0};
  float RightRumble{0};
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
  bool IsXbox{0};
  uint8_t Type{0};

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

}  // namespace mrc
