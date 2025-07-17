// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>
#include <thread>

#include <frc/apriltag/AprilTagFieldLayout.h>

#include "cameracalibration.h"

namespace wpical {
std::optional<frc::AprilTagFieldLayout> calibrate(
    std::string inputDirPath, wpical::CameraModel& cameraModel,
    const frc::AprilTagFieldLayout& idealLayout, int pinnedTagId,
    bool showDebugWindow);

class FieldCalibrator {
 public:
  bool IsFinished() { return m_isFinished; }

  std::optional<frc::AprilTagFieldLayout> GetAprilTagFieldLayout() {
    return m_fieldLayout;
  }

  void Calibrate(std::string inputDirPath, wpical::CameraModel& cameraModel,
                 const frc::AprilTagFieldLayout& idealLayout, int pinnedTagId,
                 bool showDebugWindow) {
    m_processingThread = std::thread([=, this]() mutable {
      this->m_fieldLayout = calibrate(inputDirPath, cameraModel, idealLayout,
                                      pinnedTagId, showDebugWindow);
      this->m_isFinished = true;
    });
  }

 private:
  std::atomic_bool m_isFinished{false};
  std::thread m_processingThread;
  std::optional<frc::AprilTagFieldLayout> m_fieldLayout;
};
}  // namespace wpical
