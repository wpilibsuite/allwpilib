// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/opmode/LinearOpMode.hpp"

#include "wpi/hal/DriverStation.h"
#include "wpi/internal/DriverStationModeThread.hpp"

using namespace wpi;

void LinearOpMode::OpModeRun(int64_t opModeId) {
  auto word = wpi::hal::GetControlWord();
  word.SetOpModeId(opModeId);
  internal::DriverStationModeThread bgThread{word};
  Run();
}

void LinearOpMode::OpModeStop() {
  m_running = false;
}
