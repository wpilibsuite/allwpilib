// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/opmode/PeriodicOpMode.hpp"

#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/RobotController.hpp"

using namespace wpi;

PeriodicOpMode::PeriodicOpMode()
    : m_startTime{std::chrono::microseconds{RobotController::GetFPGATime()}} {
  HAL_ReportUsage("OpMode", "PeriodicOpMode");
}

void PeriodicOpMode::AddPeriodic(const std::function<void()>& callback,
                                 wpi::units::second_t period,
                                 wpi::units::second_t offset) {
  m_callbacks.emplace_back(
      callback, m_startTime,
      std::chrono::microseconds{static_cast<int64_t>(period.value() * 1e6)},
      std::chrono::microseconds{static_cast<int64_t>(offset.value() * 1e6)});
}
