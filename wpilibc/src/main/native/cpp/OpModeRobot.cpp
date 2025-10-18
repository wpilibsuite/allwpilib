// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/OpModeRobot.h"

#include <hal/DriverStation.h>

#include "frc/DriverStation.h"

using namespace frc;

void OpModeRobotBase::StartCompetition() {
  // TODO
}

void OpModeRobotBase::EndCompetition() {
  // TODO
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description,
                                       const Color& textColor,
                                       const Color& backgroundColor) {
  int64_t id = DriverStation::AddOpMode(mode, name, group, description,
                                        textColor, backgroundColor);
  if (id != 0) {
    m_opModes[id] = std::move(factory);
  }
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description) {
  int64_t id = DriverStation::AddOpMode(mode, name, group, description);
  if (id != 0) {
    m_opModes[id] = std::move(factory);
  }
}

void OpModeRobotBase::RemoveOpMode(RobotMode mode, std::string_view name) {
  int64_t id = DriverStation::RemoveOpMode(mode, name);
  if (id != 0) {
    m_opModes.erase(id);
  }
}

void OpModeRobotBase::PublishOpModes() {
  DriverStation::PublishOpModes();
}

void OpModeRobotBase::ClearOpModes() {
  DriverStation::ClearOpModes();
  m_opModes.clear();
}
