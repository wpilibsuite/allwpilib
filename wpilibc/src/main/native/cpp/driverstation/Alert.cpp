// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Alert.hpp"

#include <stdint.h>

#include <string>

#include "wpi/util/string.h"

using namespace wpi;

static HAL_AlertHandle CreateAlert(std::string_view group,
                                   std::string_view text, Alert::Level level) {
  WPI_String wpiGroup = wpi::util::make_string(group);
  WPI_String wpiText = wpi::util::make_string(text);
  int32_t status = 0;
  return HAL_CreateAlert(&wpiGroup, &wpiText, static_cast<int32_t>(level),
                         &status);
}

Alert::Alert(std::string_view text, Level type) : Alert("Alerts", text, type) {}

Alert::Alert(std::string_view group, std::string_view text, Level type)
    : m_handle{CreateAlert(group, text, type)} {}

void Alert::Set(bool active) {
  int32_t status = 0;
  HAL_SetAlertActive(m_handle, active, &status);
}

bool Alert::Get() const {
  int32_t status = 0;
  return HAL_IsAlertActive(m_handle, &status);
}

void Alert::SetText(std::string_view text) {
  WPI_String wpiText = wpi::util::make_string(text);
  int32_t status = 0;
  HAL_SetAlertText(m_handle, &wpiText, &status);
}

std::string Alert::GetText() const {
  WPI_String wpiText;
  int32_t status = 0;
  HAL_GetAlertText(m_handle, &wpiText, &status);
  std::string rv{wpiText.str, wpiText.len};
  WPI_FreeString(&wpiText);
  return rv;
}
