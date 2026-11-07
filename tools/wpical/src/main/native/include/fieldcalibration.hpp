// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>
#include <thread>

#include "cameracalibration.hpp"
#include "wpi/fields/Field.hpp"

namespace wpical {
std::optional<wpi::fields::Field> calibrate(
    std::string inputDirPath, wpical::CameraModel& cameraModel,
    const wpi::fields::Field& idealLayout, int pinnedTagId,
    bool showDebugWindow);

class FieldCalibrator {
 public:
  ~FieldCalibrator();

  bool IsFinished() { return m_isFinished; }

  std::optional<wpi::fields::Field> GetField() { return m_fieldLayout; }

  void Calibrate(std::string inputDirPath, wpical::CameraModel& cameraModel,
                 const wpi::fields::Field& idealLayout, int pinnedTagId,
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
  std::optional<wpi::fields::Field> m_fieldLayout;
};
}  // namespace wpical
