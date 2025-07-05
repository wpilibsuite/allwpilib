// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/deprecated.h>
#include <wpi/telemetry/TelemetryLoggable.h>

#include "frc/motorcontrol/MotorController.h"

WPI_IGNORE_DEPRECATED

namespace frc {

/**
 * Allows multiple MotorController objects to be linked together.
 */
class [[deprecated(
    "Use PWMMotorController::AddFollower() or if using CAN motor controllers,"
    "use their method of following.")]] MotorControllerGroup
    : public MotorController,
      public wpi::TelemetryLoggable {
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
            motorController, motorControllers...}) {}

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

  std::string_view GetTelemetryType() const override;
  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

 private:
  bool m_isInverted = false;
  std::vector<std::reference_wrapper<MotorController>> m_motorControllers;
};

}  // namespace frc

WPI_UNIGNORE_DEPRECATED
