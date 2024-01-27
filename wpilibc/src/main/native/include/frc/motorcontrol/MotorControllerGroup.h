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

WPI_IGNORE_DEPRECATED

namespace frc {

/**
 * Allows multiple MotorController objects to be linked together.
 */
class [[deprecated(
    "Use PWMMotorController::AddFollower() or if using CAN motor controllers,"
    "use their method of following.")]] MotorControllerGroup
    : public wpi::Sendable,
      public MotorController,
      public wpi::SendableHelper<MotorControllerGroup> {
 public:
  /**
   * Create a new MotorControllerGroup with the provided MotorControllers.
   *
   * @tparam MotorControllers The MotorController types.
   * @param motorController The first MotorController to add
   * @param motorControllers The MotorControllers to add
   */
  template <class... MotorControllers>
  explicit MotorControllerGroup(MotorController& motorController,
                                MotorControllers&... motorControllers);

  /**
   * Create a new MotorControllerGroup with the provided MotorControllers.
   *
   * @param motorControllers The MotorControllers to add.
   */
  explicit MotorControllerGroup(
      std::vector<std::reference_wrapper<MotorController>>&& motorControllers);

  MotorControllerGroup(MotorControllerGroup&&) = default;
  MotorControllerGroup& operator=(MotorControllerGroup&&) = default;

  void Set(double speed) override;
  void SetVoltage(units::volt_t output) override;
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

WPI_UNIGNORE_DEPRECATED
