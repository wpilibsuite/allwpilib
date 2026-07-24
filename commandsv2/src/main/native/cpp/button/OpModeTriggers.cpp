// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/OpModeTriggers.hpp"

#include "wpi/commands2/OpModeFetcher.hpp"
#include "wpi/driverstation/RobotState.hpp"

using namespace wpi::cmd;

OpModeTriggers::OpModeTriggers(std::string_view name)
    : m_name{name},
      m_loaded{[this] {
        return OpModeFetcher::GetFetcher().GetOpModeName() == m_name;
      }},
      m_enabled{m_loaded && [] { return wpi::RobotState::IsEnabled(); }},
      m_disabled{m_loaded && [] { return wpi::RobotState::IsDisabled(); }} {}

Trigger OpModeTriggers::Loaded() const {
  return m_loaded;
}

Trigger OpModeTriggers::Enabled() const {
  return m_enabled;
}

Trigger OpModeTriggers::Enabled(Trigger other) const {
  return Enabled() && other;
}

Trigger OpModeTriggers::Disabled() const {
  return m_disabled;
}

Trigger OpModeTriggers::Disabled(Trigger other) const {
  return Disabled() && other;
}
