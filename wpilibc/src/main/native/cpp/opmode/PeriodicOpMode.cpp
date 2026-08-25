// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/opmode/PeriodicOpMode.hpp"

#include <utility>

#include "wpi/system/RobotController.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

PeriodicOpMode::PeriodicOpMode()
    : m_startTime{
          std::chrono::nanoseconds{RobotController::GetMonotonicTime()}} {
  wpi::util::ReportUsage("OpMode", "PeriodicOpMode");
}

void PeriodicOpMode::AddPeriodic(std::function<void()> callback,
                                 wpi::units::seconds<> period,
                                 wpi::units::seconds<> offset) {
  m_callbacks.emplace_back(
      std::move(callback), m_startTime,
      std::chrono::nanoseconds{static_cast<int64_t>(period.value() * 1e9)},
      std::chrono::nanoseconds{static_cast<int64_t>(offset.value() * 1e9)});
}
