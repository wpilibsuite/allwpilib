// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <string>

#include <hal/SimDevice.h>
#include <wpi/StringMap.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"

namespace frc {
class Mechanism2D {
 public:
  Mechanism2D();

  /**
   * Set/Create the angle of a ligament
   *
   * @param ligamentPath json path to the ligament
   * @param angle to set the ligament
   */
  void SetLigamentAngle(const wpi::Twine& ligamentPath, float angle);

  /**
   * Set/Create the length of a ligament
   *
   * @param ligamentPath json path to the ligament
   * @param length to set the ligament
   */
  void SetLigamentLength(const wpi::Twine& ligamentPath, float length);

 private:
  wpi::StringMap<hal::SimDouble> createdItems;
  hal::SimDevice m_device;
};

}  // namespace frc
