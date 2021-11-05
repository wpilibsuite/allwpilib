// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/deprecated.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/motorcontrol/MotorController.h"

namespace frc {

/**
 * Allows multiple SpeedController objects to be linked together.
 *
 * @deprecated Use MotorControllerGroup.
 */
class WPI_DEPRECATED("use MotorControllerGroup") SpeedControllerGroup
    : public wpi::Sendable,
      public MotorController,
      public wpi::SendableHelper<SpeedControllerGroup> {
 public:
  template <class... SpeedControllers>
  explicit SpeedControllerGroup(SpeedController& speedController,
                                SpeedControllers&... speedControllers);
  explicit SpeedControllerGroup(
      std::vector<std::reference_wrapper<SpeedController>>&& speedControllers);

  SpeedControllerGroup(SpeedControllerGroup&&) = default;
  SpeedControllerGroup& operator=(SpeedControllerGroup&&) = default;

  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
  std::vector<std::reference_wrapper<SpeedController>> m_speedControllers;

  void Initialize();
};

}  // namespace frc

#include "frc/SpeedControllerGroup.inc"
