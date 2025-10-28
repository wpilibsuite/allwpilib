// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/util/deprecated.hpp>
#include <wpi/util/sendable/Sendable.hpp>
#include <wpi/util/sendable/SendableHelper.hpp>

#include "wpi/hardware/motor/MotorController.hpp"

WPI_IGNORE_DEPRECATED

namespace wpi {

/**
 * Allows multiple MotorController objects to be linked together.
 */
class [[deprecated(
    "Use PWMMotorController::AddFollower() or if using CAN motor controllers,"
    "use their method of following.")]] MotorControllerGroup
    : public wpi::util::Sendable,
      public MotorController,
      public wpi::util::SendableHelper<MotorControllerGroup> {
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
                                MotorControllers&... motorControllers)
      : m_motorControllers(std::vector<std::reference_wrapper<MotorController>>{
            motorController, motorControllers...}) {
    Initialize();
  }

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
  void SetVoltage(wpi::units::volt_t output) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
  std::vector<std::reference_wrapper<MotorController>> m_motorControllers;

  void Initialize();
};

}  // namespace wpi

WPI_UNIGNORE_DEPRECATED
