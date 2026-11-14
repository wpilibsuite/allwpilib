// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cstddef>
#include <format>
#include <string_view>
#include <utility>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/util/Alert.hpp"

using namespace wpi::internal;

static constexpr std::array<JoystickResourceAlert,
                            sizeof(JoystickAlerts::resourceAlertValues) /
                                sizeof(int)>
    JOYSTICK_RESOURCE_ALERTS{JoystickResourceAlert::BUTTON,
                             JoystickResourceAlert::AXIS,
                             JoystickResourceAlert::POV};

static constexpr size_t JoystickResourceAlertIndex(JoystickResourceAlert type) {
  return static_cast<size_t>(type);
}

static constexpr std::array<std::string_view, JOYSTICK_RESOURCE_ALERTS.size()>
    JOYSTICK_RESOURCE_ALERT_ID_SUFFIXES{"ButtonUnavailable", "AxisUnavailable",
                                        "POVUnavailable"};

static constexpr std::array<std::string_view, JOYSTICK_RESOURCE_ALERTS.size()>
    JOYSTICK_RESOURCE_ALERT_LABELS{"Button", "axis", "POV"};

bool JoystickAlerts::Initialize() {
  wpi::util::Alert newConnectionAlert(
      "DriverStation", std::format("joystick{}Disconnected", stick),
      std::format("Joystick on port {} not available, check if all "
                  "controllers are plugged in",
                  stick),
      wpi::util::Alert::Level::HIGH);
  if (!newConnectionAlert) {
    return false;
  }

  std::array<wpi::util::Alert, JOYSTICK_RESOURCE_ALERTS.size()>
      newResourceAlerts;
  for (auto type : JOYSTICK_RESOURCE_ALERTS) {
    auto index = JoystickResourceAlertIndex(type);
    newResourceAlerts[index] = wpi::util::Alert(
        "DriverStation",
        std::format("joystick{}{}", stick,
                    JOYSTICK_RESOURCE_ALERT_ID_SUFFIXES[index]),
        {}, wpi::util::Alert::Level::MEDIUM);
    if (!newResourceAlerts[index]) {
      return false;
    }
  }

  wpi::util::Alert newTouchpadFingerAlert(
      "DriverStation",
      std::format("joystick{}TouchpadFingerUnavailable", stick), {},
      wpi::util::Alert::Level::MEDIUM);
  if (!newTouchpadFingerAlert) {
    return false;
  }

  connectionAlert.alert = std::move(newConnectionAlert);
  for (auto type : JOYSTICK_RESOURCE_ALERTS) {
    auto index = JoystickResourceAlertIndex(type);
    resourceAlerts[index].alert = std::move(newResourceAlerts[index]);
  }
  touchpadFingerAlert.alert = std::move(newTouchpadFingerAlert);
  initialized = true;
  return true;
}

void JoystickAlerts::Release() {
  wpi::util::detail::ReleaseAlertHandle(connectionAlert.alert);
  for (auto& alertState : resourceAlerts) {
    wpi::util::detail::ReleaseAlertHandle(alertState.alert);
  }
  wpi::util::detail::ReleaseAlertHandle(touchpadFingerAlert.alert);
  initialized = false;
}

bool JoystickAlerts::PrepareForReport(bool silenceJoystickAlerts) {
  if (!wpi::RobotState::IsFMSAttached() && silenceJoystickAlerts) {
    return false;
  }

  if (DriverStationBackend::IsJoystickConnected(stick)) {
    SetAlert(connectionAlert, false);
    return true;
  }

  ClearResourceAlerts();
  SetAlert(connectionAlert, true);
  return false;
}

void JoystickAlerts::Refresh() {
  if (!initialized) {
    return;
  }

  if (!DriverStationBackend::IsJoystickConnected(stick)) {
    ClearResourceAlerts();
    return;
  }

  if (connectionAlert.active) {
    SetAlert(connectionAlert, false);
  }

  for (auto type : JOYSTICK_RESOURCE_ALERTS) {
    RefreshResourceAlert(type);
  }

  HAL_JoystickTouchpads touchpads;
  HAL_GetJoystickTouchpads(stick, &touchpads);
  int touchpad = touchpadFingerAlertTouchpad;
  int finger = touchpadFingerAlertFinger;
  if (touchpadFingerAlert.active && touchpad < touchpads.count &&
      finger < touchpads.touchpads[touchpad].count) {
    SetAlert(touchpadFingerAlert, false);
  }
}

void JoystickAlerts::ClearAllAlerts() {
  if (!initialized) {
    return;
  }

  SetAlert(connectionAlert, false);
  ClearResourceAlerts();
}

void JoystickAlerts::ClearResourceAlerts() {
  for (auto& alertState : resourceAlerts) {
    SetAlert(alertState, false);
  }
  SetAlert(touchpadFingerAlert, false);
}

void JoystickAlerts::RefreshResourceAlert(JoystickResourceAlert type) {
  auto index = JoystickResourceAlertIndex(type);
  if (resourceAlerts[index].active && IsResourceAlertAvailable(type)) {
    SetAlert(resourceAlerts[index], false);
  }
}

bool JoystickAlerts::IsResourceAlertAvailable(
    JoystickResourceAlert type) const {
  auto index = JoystickResourceAlertIndex(type);
  auto resource = resourceAlertValues[index];
  switch (type) {
    case JoystickResourceAlert::BUTTON: {
      HAL_JoystickButtons buttons;
      HAL_GetJoystickButtons(stick, &buttons);
      return (buttons.available & (1LLU << resource)) != 0;
    }
    case JoystickResourceAlert::AXIS: {
      HAL_JoystickAxes axes;
      HAL_GetJoystickAxes(stick, &axes);
      return (axes.available & (1 << resource)) != 0;
    }
    case JoystickResourceAlert::POV: {
      HAL_JoystickPOVs povs;
      HAL_GetJoystickPOVs(stick, &povs);
      return (povs.available & (1 << resource)) != 0;
    }
  }
  return false;
}

void JoystickAlerts::ReportResourceWarning(JoystickResourceAlert type,
                                           int resource) {
  auto index = JoystickResourceAlertIndex(type);
  resourceAlertValues[index] = resource;
  SetAlert(resourceAlerts[index], true,
           std::format("Joystick {} {} on port {} not available",
                       JOYSTICK_RESOURCE_ALERT_LABELS[index], resource, stick));
}

void JoystickAlerts::ReportTouchpadFingerWarning(int touchpad, int finger) {
  touchpadFingerAlertTouchpad = touchpad;
  touchpadFingerAlertFinger = finger;
  SetAlert(touchpadFingerAlert, true,
           std::format("Joystick touchpad finger {} on touchpad {} on port {} "
                       "not available",
                       finger, touchpad, stick));
}

bool JoystickAlerts::SetAlert(JoystickAlertState& alertState, bool active,
                              std::string_view alertText) {
  if (!active && !alertState.active) {
    return true;
  }

  if (active && !initialized && !Initialize()) {
    return false;
  }
  if (active && !alertText.empty()) {
    alertState.alert.SetText(alertText);
  }
  alertState.alert.Set(active);
  if (active && !alertState.alert.Get()) {
    if (!Initialize()) {
      return false;
    }
    if (!alertText.empty()) {
      alertState.alert.SetText(alertText);
    }
    alertState.alert.Set(true);
  }
  alertState.active = active;
  return true;
}
