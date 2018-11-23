/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

#include "frc/DigitalOutput.h"
#include "frc/ErrorBase.h"
#include "frc/MotorSafety.h"
#include "frc/PWM.h"
#include "frc/SpeedController.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * Nidec Brushless Motor.
 */
class NidecBrushless : public SendableBase,
                       public SpeedController,
                       public MotorSafety {
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

  // SpeedController interface
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

  // PIDOutput interface
  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  void PIDWrite(double output) override;

  // MotorSafety interface
  void StopMotor() override;
  void GetDescription(wpi::raw_ostream& desc) const override;

  /**
   * Gets the channel number associated with the object.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  // Sendable interface
  void InitSendable(SendableBuilder& builder) override;

 private:
  bool m_isInverted = false;
  std::atomic_bool m_disabled{false};
  DigitalOutput m_dio;
  PWM m_pwm;
  double m_speed = 0.0;
};

}  // namespace frc
