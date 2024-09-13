// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <units/voltage.h>
#include <wpi/deprecated.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/MotorSafety.h"
#include "frc/PWM.h"
#include "frc/motorcontrol/MotorController.h"

namespace frc {
class DMA;

WPI_IGNORE_DEPRECATED

/**
 * Common base class for all PWM Motor Controllers.
 */
class PWMMotorController : public MotorController,
                           public MotorSafety,
                           public wpi::Sendable,
                           public wpi::SendableHelper<PWMMotorController> {
 public:
  friend class DMA;

  PWMMotorController(PWMMotorController&&) = default;
  PWMMotorController& operator=(PWMMotorController&&) = default;

  /**
   * Set the PWM value.
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling
   * the value for the FPGA.
   *
   * @param value The speed value between -1.0 and 1.0 to set.
   */
  void Set(double value) override;

  /**
   * Sets the voltage output of the PWMMotorController. Compensates for
   * the current bus voltage to ensure that the desired voltage is output even
   * if the battery voltage is below 12V - highly useful when the voltage
   * outputs are "meaningful" (e.g. they come from a feedforward calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage
   * compensation to work properly - unlike the ordinary set function, it is not
   * "set it and forget it."
   *
   * @param output The voltage to output.
   */
  void SetVoltage(units::volt_t output) override;

  /**
   * Get the recently set value of the PWM. This value is affected by the
   * inversion property. If you want the value that is sent directly to the
   * MotorController, use PWM::GetSpeed() instead.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  double Get() const override;

  void SetInverted(bool isInverted) override;

  bool GetInverted() const override;

  void Disable() override;

  // MotorSafety interface
  void StopMotor() override;
  std::string GetDescription() const override;

  int GetChannel() const;

  /**
   * Optionally eliminate the deadband from a motor controller.
   *
   * @param eliminateDeadband If true, set the motor curve on the motor
   *                          controller to eliminate the deadband in the middle
   *                          of the range. Otherwise, keep the full range
   *                          without modifying any values.
   */
  void EnableDeadbandElimination(bool eliminateDeadband);

  /**
   * Make the given PWM motor controller follow the output of this one.
   *
   * @param follower The motor controller follower.
   */
  void AddFollower(PWMMotorController& follower);

  /**
   * Make the given PWM motor controller follow the output of this one.
   *
   * @param follower The motor controller follower.
   */
  template <std::derived_from<PWMMotorController> T>
  void AddFollower(T&& follower) {
    m_owningFollowers.emplace_back(
        std::make_unique<std::decay_t<T>>(std::forward<T>(follower)));
  }

 protected:
  /**
   * Constructor for a PWM Motor %Controller connected via PWM.
   *
   * @param name Name to use for SendableRegistry
   * @param channel The PWM channel that the controller is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  PWMMotorController(std::string_view name, int channel);

  void InitSendable(wpi::SendableBuilder& builder) override;

  /// PWM instances for motor controller.
  PWM m_pwm;

 private:
  bool m_isInverted = false;
  std::vector<PWMMotorController*> m_nonowningFollowers;
  std::vector<std::unique_ptr<PWMMotorController>> m_owningFollowers;

  PWM* GetPwm() { return &m_pwm; }
};

WPI_UNIGNORE_DEPRECATED

}  // namespace frc
