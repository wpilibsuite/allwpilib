// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/CommandOpModes.hpp"

#include "wpi/driverstation/RobotState.hpp"

using namespace wpi::cmd;
using wpi::hal::RobotMode;

OpModeTriggers CommandOpModes::CreateAutoOpMode(std::string_view name) {
  return CreateAutoOpMode(name, "");
}

OpModeTriggers CommandOpModes::CreateAutoOpMode(std::string_view name,
                                                std::string_view group) {
  return CreateAutoOpMode(name, group, "");
}

OpModeTriggers CommandOpModes::CreateAutoOpMode(std::string_view name,
                                                std::string_view group,
                                                std::string_view description) {
  wpi::RobotState::AddOpMode(RobotMode::AUTONOMOUS, name, group, description);
  return OpModeTriggers{name};
}

OpModeTriggers CommandOpModes::CreateAutoOpMode(
    std::string_view name, std::string_view group, std::string_view description,
    const wpi::util::Color& textColor,
    const wpi::util::Color& backgroundColor) {
  wpi::RobotState::AddOpMode(RobotMode::AUTONOMOUS, name, group, description,
                             textColor, backgroundColor);
  return OpModeTriggers{name};
}

OpModeTriggers CommandOpModes::CreateTeleopOpMode(std::string_view name) {
  return CreateTeleopOpMode(name, "");
}

OpModeTriggers CommandOpModes::CreateTeleopOpMode(std::string_view name,
                                                  std::string_view group) {
  return CreateTeleopOpMode(name, group, "");
}

OpModeTriggers CommandOpModes::CreateTeleopOpMode(
    std::string_view name, std::string_view group,
    std::string_view description) {
  wpi::RobotState::AddOpMode(RobotMode::TELEOPERATED, name, group, description);
  return OpModeTriggers{name};
}

OpModeTriggers CommandOpModes::CreateTeleopOpMode(
    std::string_view name, std::string_view group, std::string_view description,
    const wpi::util::Color& textColor,
    const wpi::util::Color& backgroundColor) {
  wpi::RobotState::AddOpMode(RobotMode::TELEOPERATED, name, group, description,
                             textColor, backgroundColor);
  return OpModeTriggers{name};
}

OpModeTriggers CommandOpModes::CreateUtilityOpMode(std::string_view name) {
  return CreateUtilityOpMode(name, "");
}

OpModeTriggers CommandOpModes::CreateUtilityOpMode(std::string_view name,
                                                   std::string_view group) {
  return CreateUtilityOpMode(name, group, "");
}

OpModeTriggers CommandOpModes::CreateUtilityOpMode(
    std::string_view name, std::string_view group,
    std::string_view description) {
  wpi::RobotState::AddOpMode(RobotMode::UTILITY, name, group, description);
  return OpModeTriggers{name};
}

OpModeTriggers CommandOpModes::CreateUtilityOpMode(
    std::string_view name, std::string_view group, std::string_view description,
    const wpi::util::Color& textColor,
    const wpi::util::Color& backgroundColor) {
  wpi::RobotState::AddOpMode(RobotMode::UTILITY, name, group, description,
                             textColor, backgroundColor);
  return OpModeTriggers{name};
}
