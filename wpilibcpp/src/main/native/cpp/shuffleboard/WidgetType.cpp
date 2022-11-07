// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/WidgetType.h"

using namespace frc;

std::string_view WidgetType::GetWidgetName() const {
  return m_widgetName;
}
