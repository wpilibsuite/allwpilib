// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/motorcontrol/MotorController.h"

namespace frc {

/**
 * Allows multiple MotorController objects to be linked together.
 */
class MotorControllerGroup : public wpi::Sendable,
                             public MotorController,
                             public wpi::SendableHelper<MotorControllerGroup> {
 public:
  template <class... MotorControllers>
  explicit MotorControllerGroup(MotorController& motorController,
                                MotorControllers&... motorControllers);
  explicit MotorControllerGroup(
      std::vector<std::reference_wrapper<MotorController>>&& motorControllers);

  MotorControllerGroup(MotorControllerGroup&&) = default;
  MotorControllerGroup& operator=(MotorControllerGroup&&) = default;

  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
  std::vector<std::reference_wrapper<MotorController>> m_motorControllers;

  void Initialize();
};

}  // namespace frc

#include "frc/motorcontrol/MotorControllerGroup.inc"
