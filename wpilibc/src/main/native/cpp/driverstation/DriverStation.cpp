// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStation.hpp"

#include <stdint.h>

#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Power.h"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi;

static constexpr int availableToCount(uint64_t available) {
  return 64 - std::countl_zero(available);
}

namespace {
// A simple class which caches the previous value written to an NT entry
// Used to prevent redundant, repeated writes of the same value
template <typename Topic>
class MatchDataSenderEntry {
 public:
  MatchDataSenderEntry(
      const std::shared_ptr<wpi::nt::NetworkTable>& table, std::string_view key,
      typename Topic::ParamType initialVal,
      wpi::util::json topicProperties = wpi::util::json::object())
      : publisher{Topic{table->GetTopic(key)}.PublishEx(Topic::kTypeString,
                                                        topicProperties)},
        prevVal{initialVal} {
    publisher.Set(initialVal);
  }

  void Set(typename Topic::ParamType val) {
    if (val != prevVal) {
      publisher.Set(val);
      prevVal = val;
    }
  }

 private:
  typename Topic::PublisherType publisher;
  typename Topic::ValueType prevVal;
};

static constexpr std::string_view kSmartDashboardType = "FMSInfo";

struct MatchDataSender {
  MatchDataSender()
      : controlWord{table->GetStructTopic<wpi::hal::ControlWord>("ControlWord")
                        .Publish()},
        opMode{table->GetStringTopic("OpMode").Publish()} {
    controlWord.Set(prevControlWord);
    opMode.Set("");
  }

  std::shared_ptr<wpi::nt::NetworkTable> table =
      wpi::nt::NetworkTableInstance::GetDefault().GetTable("FMSInfo");
  MatchDataSenderEntry<wpi::nt::StringTopic> typeMetaData{
      table,
      ".type",
      kSmartDashboardType,
      {{"SmartDashboard", kSmartDashboardType}}};
  MatchDataSenderEntry<wpi::nt::StringTopic> gameSpecificMessage{
      table, "GameSpecificMessage", ""};
  MatchDataSenderEntry<wpi::nt::StringTopic> eventName{table, "EventName", ""};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> matchNumber{table, "MatchNumber",
                                                          0};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> replayNumber{table,
                                                           "ReplayNumber", 0};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> matchType{table, "MatchType", 0};
  MatchDataSenderEntry<wpi::nt::BooleanTopic> alliance{table, "IsRedAlliance",
                                                       true};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> station{table, "StationNumber",
                                                      1};
  wpi::nt::StructPublisher<wpi::hal::ControlWord> controlWord;
  wpi::nt::StringPublisher opMode;
  wpi::hal::ControlWord prevControlWord;
};

class JoystickLogSender {
 public:
  void Init(wpi::log::DataLog& log, unsigned int stick, int64_t timestamp);
  void Send(uint64_t timestamp);

 private:
  void AppendButtons(HAL_JoystickButtons buttons, uint64_t timestamp);
  void AppendPOVs(const HAL_JoystickPOVs& povs, uint64_t timestamp);

  unsigned int m_stick;
  HAL_JoystickButtons m_prevButtons;
  HAL_JoystickAxes m_prevAxes;
  HAL_JoystickPOVs m_prevPOVs;
  wpi::log::BooleanArrayLogEntry m_logButtons;
  wpi::log::FloatArrayLogEntry m_logAxes;
  wpi::log::IntegerArrayLogEntry m_logPOVs;
};

class DataLogSender {
 public:
  void Init(wpi::log::DataLog& log, bool logJoysticks, int64_t timestamp);
  void Send(uint64_t timestamp);

 private:
  std::atomic_bool m_initialized{false};

  hal::ControlWord m_prevControlWord;
  wpi::log::StructLogEntry<hal::ControlWord> m_logControlWord;
  wpi::log::StringLogEntry m_logOpMode;

  bool m_logJoysticks;
  std::array<JoystickLogSender, DriverStation::kJoystickPorts> m_joysticks;
};

struct Instance {
  Instance();
  ~Instance();

  wpi::util::EventVector refreshEvents;
  MatchDataSender matchDataSender;
  std::atomic<DataLogSender*> dataLogSender{nullptr};

  // Joystick button rising/falling edge flags
  wpi::util::mutex buttonEdgeMutex;
  std::array<HAL_JoystickButtons, DriverStation::kJoystickPorts>
      previousButtonStates;
  std::array<uint32_t, DriverStation::kJoystickPorts> joystickButtonsPressed;
  std::array<uint32_t, DriverStation::kJoystickPorts> joystickButtonsReleased;

  bool silenceJoystickWarning = false;

  // Op mode lookup
  wpi::util::mutex opModeMutex;
  wpi::util::DenseMap<int64_t, HAL_OpModeOption> opModes;

  wpi::units::second_t nextMessageTime = 0_s;

  std::string OpModeToString(int64_t id) {
    std::scoped_lock lock{opModeMutex};
    if (id == 0) {
      return "";
    }
    auto it = opModes.find(id);
    if (it != opModes.end()) {
      return std::string{wpi::util::to_string_view(&it->second.name)};
    }
    return fmt::format("<{}>", id);
  }
};
}  // namespace

static constexpr auto kJoystickUnpluggedMessageInterval = 1_s;

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

static void SendMatchData();

template <typename S, typename... Args>
static inline void ReportJoystickUnpluggedError(const S& format,
                                                Args&&... args) {
  ReportJoystickUnpluggedErrorV(format, fmt::make_format_args(args...));
}

/**
 * Reports errors related to unplugged joysticks.
 *
 * Throttles the errors so that they don't overwhelm the DS.
 */
static void ReportJoystickUnpluggedWarningV(fmt::string_view format,
                                            fmt::format_args args);

template <typename S, typename... Args>
static inline void ReportJoystickUnpluggedWarning(const S& format,
                                                  Args&&... args) {
  ReportJoystickUnpluggedWarningV(format, fmt::make_format_args(args...));
}

Instance::Instance() {
  HAL_Initialize(500, 0);

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to motor controllers.
  for (unsigned int i = 0; i < DriverStation::kJoystickPorts; i++) {
    joystickButtonsPressed[i] = 0;
    joystickButtonsReleased[i] = 0;
    previousButtonStates[i].available = 0;
    previousButtonStates[i].buttons = 0;
  }
}

Instance::~Instance() {
  if (dataLogSender) {
    delete dataLogSender.load();
  }
}

bool DriverStation::GetStickButton(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  uint64_t mask = 1LLU << button;

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }

  return (buttons.buttons & mask) != 0;
}

std::optional<bool> DriverStation::GetStickButtonIfAvailable(int stick,
                                                             int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  uint64_t mask = 1LLU << button;

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if ((buttons.available & mask) == 0) {
    return std::nullopt;
  }

  return (buttons.buttons & mask) != 0;
}

bool DriverStation::GetStickButtonPressed(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  uint64_t mask = 1LLU << button;

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }
  auto& inst = ::GetInstance();
  std::unique_lock lock(inst.buttonEdgeMutex);
  // If button was pressed, clear flag and return true
  if (inst.joystickButtonsPressed[stick] & mask) {
    inst.joystickButtonsPressed[stick] &= ~mask;
    return true;
  }
  return false;
}

bool DriverStation::GetStickButtonReleased(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  uint64_t mask = 1LLU << button;

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }
  auto& inst = ::GetInstance();
  std::unique_lock lock(inst.buttonEdgeMutex);
  // If button was released, clear flag and return true
  if (inst.joystickButtonsReleased[stick] & mask) {
    inst.joystickButtonsReleased[stick] &= ~mask;
    return true;
  }
  return false;
}

double DriverStation::GetStickAxis(int stick, int axis) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    WPILIB_ReportError(warn::BadJoystickAxis, "axis {} out of range", axis);
    return 0.0;
  }

  uint16_t mask = 1 << axis;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if ((axes.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Axis {} missing (available {}), check if all controllers are "
        "plugged in",
        axis, axes.available);
    return 0.0;
  }

  return axes.axes[axis];
}

DriverStation::TouchpadFinger DriverStation::GetStickTouchpadFinger(
    int stick, int touchpad, int finger) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }
  if (touchpad < 0 || touchpad >= HAL_kMaxJoystickTouchpads) {
    WPILIB_ReportError(warn::BadJoystickAxis, "touchpad {} out of range",
                       touchpad);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }
  if (finger < 0 || finger >= HAL_kMaxJoystickTouchpadFingers) {
    WPILIB_ReportError(warn::BadJoystickAxis, "finger {} out of range", finger);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }

  HAL_JoystickTouchpads touchpads;
  HAL_GetJoystickTouchpads(stick, &touchpads);

  auto touchpadCount = touchpads.count;
  if (touchpad < touchpadCount) {
    if (finger < touchpads.touchpads[touchpad].count) {
      return TouchpadFinger{
          touchpads.touchpads[touchpad].fingers[finger].down != 0,
          touchpads.touchpads[touchpad].fingers[finger].x,
          touchpads.touchpads[touchpad].fingers[finger].y};
    }
  }

  ReportJoystickUnpluggedWarning(
      "Joystick Touchpad Finger {} missing, check if all controllers are "
      "plugged in",
      touchpad);
  return TouchpadFinger{false, 0.0f, 0.0f};
}

bool DriverStation::GetStickTouchpadFingerAvailable(int stick, int touchpad,
                                                    int finger) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (touchpad < 0 || touchpad >= HAL_kMaxJoystickTouchpads) {
    WPILIB_ReportError(warn::BadJoystickAxis, "touchpad {} out of range",
                       touchpad);
    return false;
  }
  if (finger < 0 || finger >= HAL_kMaxJoystickTouchpadFingers) {
    WPILIB_ReportError(warn::BadJoystickAxis, "finger {} out of range", finger);
    return false;
  }

  HAL_JoystickTouchpads touchpads;
  HAL_GetJoystickTouchpads(stick, &touchpads);

  auto touchpadCount = touchpads.count;
  if (touchpad < touchpadCount) {
    if (finger < touchpads.touchpads[touchpad].count) {
      return true;
    }
  }

  return false;
}

std::optional<double> DriverStation::GetStickAxisIfAvailable(int stick,
                                                             int axis) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    WPILIB_ReportError(warn::BadJoystickAxis, "axis {} out of range", axis);
    return 0.0;
  }

  uint16_t mask = 1 << axis;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if ((axes.available & mask) == 0) {
    return std::nullopt;
  }

  return axes.axes[axis];
}

DriverStation::POVDirection DriverStation::GetStickPOV(int stick, int pov) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return kCenter;
  }
  if (pov < 0 || pov >= HAL_kMaxJoystickPOVs) {
    WPILIB_ReportError(warn::BadJoystickAxis, "POV {} out of range", pov);
    return kCenter;
  }

  uint16_t mask = 1 << pov;

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  if ((povs.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick POV {} missing (available {}), check if all controllers are "
        "plugged in",
        pov, povs.available);
    return kCenter;
  }

  return static_cast<POVDirection>(povs.povs[pov]);
}

uint64_t DriverStation::GetStickButtons(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.buttons;
}

int DriverStation::GetStickAxesMaximumIndex(int stick) {
  return availableToCount(GetStickAxesAvailable(stick));
}

int DriverStation::GetStickAxesAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  return axes.available;
}

int DriverStation::GetStickPOVsMaximumIndex(int stick) {
  return availableToCount(GetStickPOVsAvailable(stick));
}

int DriverStation::GetStickPOVsAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  return povs.available;
}

int DriverStation::GetStickButtonsMaximumIndex(int stick) {
  return availableToCount(GetStickButtonsAvailable(stick));
}

uint64_t DriverStation::GetStickButtonsAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.available;
}

bool DriverStation::GetJoystickIsGamepad(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<bool>(descriptor.isGamepad);
}

int DriverStation::GetJoystickGamepadType(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return -1;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.gamepadType);
}

int DriverStation::GetJoystickSupportedOutputs(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.supportedOutputs);
}

std::string DriverStation::GetJoystickName(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return descriptor.name;
}

bool DriverStation::IsJoystickConnected(int stick) {
  return GetStickAxesAvailable(stick) != 0 ||
         GetStickButtonsAvailable(stick) != 0 ||
         GetStickPOVsAvailable(stick) != 0;
}

static int64_t DoAddOpMode(RobotMode mode, std::string_view name,
                           std::string_view group, std::string_view description,
                           int32_t textColor, int32_t backgroundColor) {
  if (wpi::util::trim(name).empty()) {
    return 0;
  }

  WPI_String nameWpi = wpi::util::make_string(name);
  WPI_String groupWpi = wpi::util::make_string(group);
  WPI_String descriptionWpi = wpi::util::make_string(description);

  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  std::string nameCopy{name};
  for (;;) {
    int64_t id = HAL_MakeOpModeId(static_cast<HAL_RobotMode>(mode),
                                  std::hash<std::string_view>{}(nameCopy));
    auto [it, isNew] = inst.opModes.try_emplace(
        id, HAL_OpModeOption{id, nameWpi, groupWpi, descriptionWpi, textColor,
                             backgroundColor});
    if (isNew) {
      return id;
    }
    if (HAL_OpMode_GetRobotMode(it->second.id) ==
            static_cast<HAL_RobotMode>(mode) &&
        wpi::util::to_string_view(&it->second.name) == name) {
      return 0;  // can't insert duplicate name
    }
    // collision, try again with space appended
    nameCopy += ' ';
  }
}

static int32_t ConvertColorToInt(const wpi::util::Color& color) {
  return ((static_cast<int32_t>(color.red * 255) & 0xff) << 16) |
         ((static_cast<int32_t>(color.green * 255) & 0xff) << 8) |
         (static_cast<int32_t>(color.blue * 255) & 0xff);
}

int64_t DriverStation::AddOpMode(RobotMode mode, std::string_view name,
                                 std::string_view group,
                                 std::string_view description,
                                 const wpi::util::Color& textColor,
                                 const wpi::util::Color& backgroundColor) {
  return DoAddOpMode(mode, name, group, description,
                     ConvertColorToInt(textColor),
                     ConvertColorToInt(backgroundColor));
}

int64_t DriverStation::AddOpMode(RobotMode mode, std::string_view name,
                                 std::string_view group,
                                 std::string_view description) {
  return DoAddOpMode(mode, name, group, description, -1, -1);
}

int64_t DriverStation::RemoveOpMode(RobotMode mode, std::string_view name) {
  if (wpi::util::trim(name).empty()) {
    return 0;
  }

  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  // we have to loop over all entries to find the one with the correct name
  // because the of the unique ID generation scheme
  for (auto it = inst.opModes.begin(), end = inst.opModes.end(); it != end;
       ++it) {
    if (HAL_OpMode_GetRobotMode(it->second.id) ==
            static_cast<HAL_RobotMode>(mode) &&
        wpi::util::to_string_view(&it->second.name) == name) {
      int64_t id = it->second.id;
      inst.opModes.erase(it);
      return id;
    }
  }
  return 0;
}

void DriverStation::PublishOpModes() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  std::vector<HAL_OpModeOption> options;
  options.reserve(inst.opModes.size());
  for (auto&& [id, option] : inst.opModes) {
    options.emplace_back(option);
  }
  HAL_SetOpModeOptions(options.data(), options.size());
}

void DriverStation::ClearOpModes() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  inst.opModes.clear();
  HAL_SetOpModeOptions(nullptr, 0);
}

std::string DriverStation::GetOpMode() {
  return GetInstance().OpModeToString(GetOpModeId());
}

std::string DriverStation::GetGameSpecificMessage() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return std::string(reinterpret_cast<char*>(info.gameSpecificMessage),
                     info.gameSpecificMessageSize);
}

std::string DriverStation::GetEventName() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.eventName;
}

DriverStation::MatchType DriverStation::GetMatchType() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return static_cast<DriverStation::MatchType>(info.matchType);
}

int DriverStation::GetMatchNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.matchNumber;
}

int DriverStation::GetReplayNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.replayNumber;
}

std::optional<DriverStation::Alliance> DriverStation::GetAlliance() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kRed3:
      return kRed;
    case HAL_AllianceStationID_kBlue1:
    case HAL_AllianceStationID_kBlue2:
    case HAL_AllianceStationID_kBlue3:
      return kBlue;
    default:
      return {};
  }
}

std::optional<int> DriverStation::GetLocation() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kBlue1:
      return 1;
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kBlue2:
      return 2;
    case HAL_AllianceStationID_kRed3:
    case HAL_AllianceStationID_kBlue3:
      return 3;
    default:
      return {};
  }
}

wpi::units::second_t DriverStation::GetMatchTime() {
  int32_t status = 0;
  return wpi::units::second_t{HAL_GetMatchTime(&status)};
}

double DriverStation::GetBatteryVoltage() {
  int32_t status = 0;
  double voltage = HAL_GetVinVoltage(&status);
  WPILIB_CheckErrorStatus(status, "getVinVoltage");

  return voltage;
}

/**
 * Copy data from the DS task for the user.
 *
 * If no new data exists, it will just be returned, otherwise
 * the data will be copied from the DS polling loop.
 */
void DriverStation::RefreshData() {
  HAL_RefreshDSData();
  auto& inst = ::GetInstance();
  {
    // Compute the pressed and released buttons
    HAL_JoystickButtons currentButtons;
    std::unique_lock lock(inst.buttonEdgeMutex);

    for (int32_t i = 0; i < DriverStation::kJoystickPorts; i++) {
      HAL_GetJoystickButtons(i, &currentButtons);

      // If buttons weren't pressed and are now, set flags in m_buttonsPressed
      inst.joystickButtonsPressed[i] |=
          ~inst.previousButtonStates[i].buttons & currentButtons.buttons;

      // If buttons were pressed and aren't now, set flags in m_buttonsReleased
      inst.joystickButtonsReleased[i] |=
          inst.previousButtonStates[i].buttons & ~currentButtons.buttons;

      inst.previousButtonStates[i] = currentButtons;
    }
  }

  inst.refreshEvents.Wakeup();

  SendMatchData();
  if (auto sender = inst.dataLogSender.load()) {
    sender->Send(wpi::util::Now());
  }
}

void DriverStation::ProvideRefreshedDataEventHandle(WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Add(handle);
}

void DriverStation::RemoveRefreshedDataEventHandle(WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Remove(handle);
}

void DriverStation::SilenceJoystickConnectionWarning(bool silence) {
  ::GetInstance().silenceJoystickWarning = silence;
}

bool DriverStation::IsJoystickConnectionWarningSilenced() {
  return !IsFMSAttached() && ::GetInstance().silenceJoystickWarning;
}

void DriverStation::StartDataLog(wpi::log::DataLog& log, bool logJoysticks) {
  auto& inst = ::GetInstance();
  // Note: cannot safely replace, because we wouldn't know when to delete the
  // "old" one. Instead do a compare and exchange with nullptr. We check first
  // with a simple load to avoid the new in the common case.
  if (inst.dataLogSender.load()) {
    return;
  }
  DataLogSender* oldSender = nullptr;
  DataLogSender* newSender = new DataLogSender;
  inst.dataLogSender.compare_exchange_strong(oldSender, newSender);
  if (oldSender) {
    delete newSender;  // already had a sender
  } else {
    newSender->Init(log, logJoysticks, wpi::util::Now());
  }
}

void ReportJoystickUnpluggedWarningV(fmt::string_view format,
                                     fmt::format_args args) {
  auto& inst = GetInstance();
  if (DriverStation::IsFMSAttached() || !inst.silenceJoystickWarning) {
    auto currentTime = Timer::GetTimestamp();
    if (currentTime > inst.nextMessageTime) {
      ReportErrorV(warn::Warning, "", 0, "", format, args);
      inst.nextMessageTime = currentTime + kJoystickUnpluggedMessageInterval;
    }
  }
}

void SendMatchData() {
  int32_t status = 0;
  HAL_AllianceStationID alliance = HAL_GetAllianceStation(&status);
  bool isRedAlliance = false;
  int stationNumber = 1;
  switch (alliance) {
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue1:
      isRedAlliance = false;
      stationNumber = 1;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue2:
      isRedAlliance = false;
      stationNumber = 2;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue3:
      isRedAlliance = false;
      stationNumber = 3;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kRed1:
      isRedAlliance = true;
      stationNumber = 1;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kRed2:
      isRedAlliance = true;
      stationNumber = 2;
      break;
    default:
      isRedAlliance = true;
      stationNumber = 3;
      break;
  }

  HAL_MatchInfo tmpDataStore;
  HAL_GetMatchInfo(&tmpDataStore);

  auto& inst = GetInstance();
  inst.matchDataSender.alliance.Set(isRedAlliance);
  inst.matchDataSender.station.Set(stationNumber);
  inst.matchDataSender.eventName.Set(tmpDataStore.eventName);
  inst.matchDataSender.gameSpecificMessage.Set(
      std::string(reinterpret_cast<char*>(tmpDataStore.gameSpecificMessage),
                  tmpDataStore.gameSpecificMessageSize));
  inst.matchDataSender.matchNumber.Set(tmpDataStore.matchNumber);
  inst.matchDataSender.replayNumber.Set(tmpDataStore.replayNumber);
  inst.matchDataSender.matchType.Set(static_cast<int>(tmpDataStore.matchType));

  hal::ControlWord ctlWord = hal::GetControlWord();
  if (ctlWord != inst.matchDataSender.prevControlWord) {
    int64_t opModeId = ctlWord.GetOpModeId();
    if (opModeId != inst.matchDataSender.prevControlWord.GetOpModeId()) {
      inst.matchDataSender.opMode.Set(inst.OpModeToString(opModeId));
    }

    inst.matchDataSender.prevControlWord = ctlWord;
    inst.matchDataSender.controlWord.Set(ctlWord);
  }
}

void JoystickLogSender::Init(wpi::log::DataLog& log, unsigned int stick,
                             int64_t timestamp) {
  m_stick = stick;

  m_logButtons = wpi::log::BooleanArrayLogEntry{
      log, fmt::format("DS:joystick{}/buttons", stick), timestamp};
  m_logAxes = wpi::log::FloatArrayLogEntry{
      log, fmt::format("DS:joystick{}/axes", stick), timestamp};
  m_logPOVs = wpi::log::IntegerArrayLogEntry{
      log, fmt::format("DS:joystick{}/povs", stick), timestamp};

  HAL_GetJoystickButtons(m_stick, &m_prevButtons);
  HAL_GetJoystickAxes(m_stick, &m_prevAxes);
  HAL_GetJoystickPOVs(m_stick, &m_prevPOVs);
  AppendButtons(m_prevButtons, timestamp);
  int axesCount = availableToCount(m_prevAxes.available);
  m_logAxes.Append(
      std::span<const float>{m_prevAxes.axes, static_cast<size_t>(axesCount)},
      timestamp);
  AppendPOVs(m_prevPOVs, timestamp);
}

void JoystickLogSender::Send(uint64_t timestamp) {
  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(m_stick, &buttons);
  if (buttons.available != m_prevButtons.available ||
      buttons.buttons != m_prevButtons.buttons) {
    AppendButtons(buttons, timestamp);
  }
  m_prevButtons = buttons;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(m_stick, &axes);
  int axesCount = availableToCount(axes.available);
  if (axes.available != m_prevAxes.available ||
      std::memcmp(axes.axes, m_prevAxes.axes,
                  sizeof(axes.axes[0]) * axesCount) != 0) {
    m_logAxes.Append(
        std::span<const float>{axes.axes, static_cast<size_t>(axesCount)},
        timestamp);
  }
  m_prevAxes = axes;

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(m_stick, &povs);
  int povsCount = availableToCount(povs.available);
  if (povs.available != m_prevPOVs.available ||
      std::memcmp(povs.povs, m_prevPOVs.povs,
                  sizeof(povs.povs[0]) * povsCount) != 0) {
    AppendPOVs(povs, timestamp);
  }
  m_prevPOVs = povs;
}

void JoystickLogSender::AppendButtons(HAL_JoystickButtons buttons,
                                      uint64_t timestamp) {
  int count = availableToCount(buttons.available);
  uint8_t buttonsArr[64];
  for (int i = 0; i < count; ++i) {
    buttonsArr[i] = (buttons.buttons & (1llu << i)) != 0;
  }
  m_logButtons.Append(
      std::span<const uint8_t>{buttonsArr, static_cast<size_t>(count)},
      timestamp);
}

void JoystickLogSender::AppendPOVs(const HAL_JoystickPOVs& povs,
                                   uint64_t timestamp) {
  int count = availableToCount(povs.available);
  int64_t povsArr[HAL_kMaxJoystickPOVs];
  for (int i = 0; i < count; ++i) {
    povsArr[i] = povs.povs[i];
  }
  m_logPOVs.Append(
      std::span<const int64_t>{povsArr, static_cast<size_t>(count)}, timestamp);
}

void DataLogSender::Init(wpi::log::DataLog& log, bool logJoysticks,
                         int64_t timestamp) {
  m_logControlWord = wpi::log::StructLogEntry<hal::ControlWord>{
      log, "DS:controlWord", timestamp};
  m_logOpMode = wpi::log::StringLogEntry{log, "DS:opMode", timestamp};

  // append initial control word value
  m_prevControlWord = hal::GetControlWord();
  m_logControlWord.Append(m_prevControlWord);

  // append initial opmode value
  auto& inst = GetInstance();
  m_logOpMode.Append(inst.OpModeToString(m_prevControlWord.GetOpModeId()));

  m_logJoysticks = logJoysticks;
  if (logJoysticks) {
    unsigned int i = 0;
    for (auto&& joystick : m_joysticks) {
      joystick.Init(log, i++, timestamp);
    }
  }

  m_initialized = true;
}

void DataLogSender::Send(uint64_t timestamp) {
  if (!m_initialized) {
    return;
  }

  // append control word value changes
  hal::ControlWord ctlWord = hal::GetControlWord();
  if (ctlWord != m_prevControlWord) {
    // append opmode value changes
    int64_t opModeId = ctlWord.GetOpModeId();
    if (opModeId != m_prevControlWord.GetOpModeId()) {
      auto& inst = GetInstance();
      m_logOpMode.Append(inst.OpModeToString(opModeId));
    }

    m_prevControlWord = ctlWord;
    m_logControlWord.Append(ctlWord);
  }

  if (m_logJoysticks) {
    // append joystick value changes
    for (auto&& joystick : m_joysticks) {
      joystick.Send(timestamp);
    }
  }
}
