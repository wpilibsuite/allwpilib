// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/opmode/LinearOpMode.h"

#include "frc/internal/DriverStationModeThread.h"
#include "hal/DriverStation.h"

using namespace frc;

void LinearOpMode::OpmodeRun(int64_t opModeId) {
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  HAL_ControlWord_SetOpModeId(&word, opModeId);

  internal::DriverStationModeThread bgThread{word};
  Start();
  Run();
  End();
}

void LinearOpMode::OpmodeStop() {
  m_running = false;
}
