// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>
#include <thread>

#include "cameracalibration.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"

namespace wpical {
std::optional<wpi::apriltag::AprilTagFieldLayout> calibrate(
    std::string inputDirPath, wpical::CameraModel& cameraModel,
    const wpi::apriltag::AprilTagFieldLayout& idealLayout, int pinnedTagId,
    bool showDebugWindow);

class FieldCalibrator {
 public:
  ~FieldCalibrator();

  bool IsFinished() { return m_isFinished; }

  std::optional<wpi::apriltag::AprilTagFieldLayout> GetAprilTagFieldLayout() {
    return m_fieldLayout;
  }

  void Calibrate(std::string inputDirPath, wpical::CameraModel& cameraModel,
                 const wpi::apriltag::AprilTagFieldLayout& idealLayout,
                 int pinnedTagId, bool showDebugWindow) {
    m_processingThread = std::thread([=, this]() mutable {
      this->m_fieldLayout = calibrate(inputDirPath, cameraModel, idealLayout,
                                      pinnedTagId, showDebugWindow);
      this->m_isFinished = true;
    });
  }

 private:
  std::atomic_bool m_isFinished{false};
  std::thread m_processingThread;
  std::optional<wpi::apriltag::AprilTagFieldLayout> m_fieldLayout;
};
}  // namespace wpical
