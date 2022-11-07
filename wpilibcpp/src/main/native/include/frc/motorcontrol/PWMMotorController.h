// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/MotorSafety.h"
#include "frc/PWM.h"
#include "frc/motorcontrol/MotorController.h"

namespace frc {
class DMA;

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
   * @param eliminateDeadband If true, set the motor curve on the speed
   *                          controller to eliminate the deadband in the middle
   *                          of the range. Otherwise, keep the full range
   *                          without modifying any values.
   */
  void EnableDeadbandElimination(bool eliminateDeadband);

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

  PWM m_pwm;

 private:
  bool m_isInverted = false;

  PWM* GetPwm() { return &m_pwm; }
};

}  // namespace frc
