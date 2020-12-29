// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/InstantCommand.h>

/**
 * This command sets the collector rollers spinning at the given speed. Since
 * there is no sensor for detecting speed, it finishes immediately. As a result,
 * the spinners may still be adjusting their speed.
 */
class SetCollectionSpeed : public frc::InstantCommand {
 public:
  explicit SetCollectionSpeed(double speed);
  void Initialize() override;

 private:
  double m_speed;
};
