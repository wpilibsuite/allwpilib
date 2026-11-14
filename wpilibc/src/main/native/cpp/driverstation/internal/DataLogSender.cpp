// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <format>
#include <span>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/DriverStationTypes.hpp"

using namespace wpi;
using namespace wpi::internal;

class JoystickLogSender {
 public:
  void Init(wpi::log::DataLog& log, unsigned int stick, int64_t timestamp);
  void Send(int64_t timestamp);

 private:
  void AppendButtons(HAL_JoystickButtons buttons, int64_t timestamp);
  void AppendPOVs(const HAL_JoystickPOVs& povs, int64_t timestamp);

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
  void Send(int64_t timestamp);

 private:
  std::atomic_bool m_initialized{false};

  hal::ControlWord m_prevControlWord;
  wpi::log::StructLogEntry<hal::ControlWord> m_logControlWord;
  wpi::log::StringLogEntry m_logOpMode;

  bool m_logJoysticks;
  std::array<JoystickLogSender, DriverStationBackend::JOYSTICK_PORTS>
      m_joysticks;
};

DataLogSender* CreateDataLogSender() {
  return new DataLogSender;
}

void InitDataLogSender(DataLogSender* sender, wpi::log::DataLog& log,
                       bool logJoysticks, int64_t timestamp) {
  sender->Init(log, logJoysticks, timestamp);
}

void SendDataLogSender(DataLogSender* sender, int64_t timestamp) {
  sender->Send(timestamp);
}

void DeleteDataLogSender(DataLogSender* sender) {
  delete sender;
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
  m_logOpMode.Append(
      wpi::RobotState::OpModeToString(m_prevControlWord.GetOpModeId()));

  m_logJoysticks = logJoysticks;
  if (logJoysticks) {
    unsigned int i = 0;
    for (auto&& joystick : m_joysticks) {
      joystick.Init(log, i++, timestamp);
    }
  }

  m_initialized = true;
}

void DataLogSender::Send(int64_t timestamp) {
  if (!m_initialized) {
    return;
  }

  // append control word value changes
  hal::ControlWord ctlWord = hal::GetControlWord();
  if (ctlWord != m_prevControlWord) {
    // append opmode value changes
    int64_t opModeId = ctlWord.GetOpModeId();
    if (opModeId != m_prevControlWord.GetOpModeId()) {
      m_logOpMode.Append(wpi::RobotState::OpModeToString(opModeId));
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

void JoystickLogSender::Init(wpi::log::DataLog& log, unsigned int stick,
                             int64_t timestamp) {
  m_stick = stick;

  m_logButtons = wpi::log::BooleanArrayLogEntry{
      log, std::format("DS:joystick{}/buttons", stick), timestamp};
  m_logAxes = wpi::log::FloatArrayLogEntry{
      log, std::format("DS:joystick{}/axes", stick), timestamp};
  m_logPOVs = wpi::log::IntegerArrayLogEntry{
      log, std::format("DS:joystick{}/povs", stick), timestamp};

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

void JoystickLogSender::Send(int64_t timestamp) {
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
                                      int64_t timestamp) {
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
                                   int64_t timestamp) {
  int count = availableToCount(povs.available);
  int64_t povsArr[HAL_MAX_JOYSTICK_POVS];
  for (int i = 0; i < count; ++i) {
    povsArr[i] = povs.povs[i];
  }
  m_logPOVs.Append(
      std::span<const int64_t>{povsArr, static_cast<size_t>(count)}, timestamp);
}
