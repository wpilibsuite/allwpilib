// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/opmode/LinearOpMode.h"

using namespace frc;

void LinearOpMode::OpmodeRun(int64_t opModeId) {
  Start();
  Run();
  End();
}

void LinearOpMode::OpmodeStop() {
  m_running = false;
}
