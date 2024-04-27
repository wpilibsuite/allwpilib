// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/deprecated.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/DigitalOutput.h"
#include "frc/MotorSafety.h"
#include "frc/PWM.h"
#include "frc/motorcontrol/MotorController.h"

namespace frc {

WPI_IGNORE_DEPRECATED

/**
 * Nidec Brushless Motor.
 */
class NidecBrushless : public MotorController,
                       public MotorSafety,
                       public wpi::Sendable,
                       public wpi::SendableHelper<NidecBrushless> {
 public:
  /**
   * Constructor.
   *
   * @param pwmChannel The PWM channel that the Nidec Brushless controller is
   *                   attached to. 0-9 are on-board, 10-19 are on the MXP port.
   * @param dioChannel The DIO channel that the Nidec Brushless controller is
   *                   attached to. 0-9 are on-board, 10-25 are on the MXP port.
   */
  NidecBrushless(int pwmChannel, int dioChannel);

  ~NidecBrushless() override = default;

  NidecBrushless(NidecBrushless&&) = default;
  NidecBrushless& operator=(NidecBrushless&&) = default;

  // MotorController interface
  /**
   * Set the PWM value.
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling
   * the value for the FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  void Set(double speed) override;

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  double Get() const override;

  void SetInverted(bool isInverted) override;

  bool GetInverted() const override;

  /**
   * Disable the motor. The Enable() function must be called to re-enable the
   * motor.
   */
  void Disable() override;

  /**
   * Re-enable the motor after Disable() has been called. The Set() function
   * must be called to set a new motor speed.
   */
  void Enable();

  // MotorSafety interface
  void StopMotor() override;
  std::string GetDescription() const override;

  /**
   * Gets the channel number associated with the object.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  // Sendable interface
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
  bool m_disabled = false;
  DigitalOutput m_dio;
  PWM m_pwm;
  double m_speed = 0.0;
};

WPI_UNIGNORE_DEPRECATED

}  // namespace frc
