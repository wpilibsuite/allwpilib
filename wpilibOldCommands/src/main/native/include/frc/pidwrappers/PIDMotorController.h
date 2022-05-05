// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/PIDOutput.h"
#include "frc/motorcontrol/MotorController.h"

namespace frc {

/**
 * Wrapper so that PIDOutput is implemented for MotorController for old
 * PIDController
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @deprecated Use frc2::PIDController class instead which doesn't require this
 * wrapper.
 */
class PIDMotorController : public PIDOutput,
                           public MotorController,
                           public wpi::Sendable {
 public:
  WPI_DEPRECATED("Use frc2::PIDController class instead.")
  explicit PIDMotorController(MotorController& motorController);

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

  void StopMotor() override;

  void PIDWrite(double output) override;

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;

  MotorController& m_motorController;
};

}  // namespace frc
