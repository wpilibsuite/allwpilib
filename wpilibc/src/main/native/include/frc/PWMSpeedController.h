// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWM.h"
#include "frc/SpeedController.h"

namespace frc {

/**
 * Common base class for all PWM Speed Controllers.
 */
class PWMSpeedController : public PWM, public SpeedController {
 public:
  PWMSpeedController(PWMSpeedController&&) = default;
  PWMSpeedController& operator=(PWMSpeedController&&) = default;

  /**
   * Set the PWM value.
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling
   * the value for the FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  void Set(double value) override;

  /**
   * Get the recently set value of the PWM. This value is affected by the
   * inversion property. If you want the value that is sent directly to the
   * SpeedController, use {@link PWM#getSpeed()} instead.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  double Get() const override;

  void SetInverted(bool isInverted) override;

  bool GetInverted() const override;

  void Disable() override;

  void StopMotor() override;

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  void PIDWrite(double output) override;

 protected:
  /**
   * Constructor for a PWM Speed Controller connected via PWM.
   *
   * @param channel The PWM channel that the controller is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit PWMSpeedController(int channel);

  void InitSendable(SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
};

}  // namespace frc
