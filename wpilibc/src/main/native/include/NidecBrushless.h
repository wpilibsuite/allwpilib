/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

#include "DigitalOutput.h"
#include "ErrorBase.h"
#include "MotorSafety.h"
#include "MotorSafetyHelper.h"
#include "PWM.h"
#include "SmartDashboard/SendableBase.h"
#include "SpeedController.h"

namespace frc {

/**
 * Nidec Brushless Motor.
 */
class NidecBrushless : public ErrorBase,
                       public SendableBase,
                       public SpeedController,
                       public MotorSafety {
 public:
  NidecBrushless(int pwmChannel, int dioChannel);
  ~NidecBrushless() override = default;

  // SpeedController interface
  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void Enable();

  // PIDOutput interface
  void PIDWrite(double output) override;

  // MotorSafety interface
  void SetExpiration(double timeout) override;
  double GetExpiration() const override;
  bool IsAlive() const override;
  void SetSafetyEnabled(bool enabled) override;
  bool IsSafetyEnabled() const override;
  void GetDescription(llvm::raw_ostream& desc) const override;

  int GetChannel() const;

  // Sendable interface
  void InitSendable(SendableBuilder& builder) override;

 private:
  MotorSafetyHelper m_safetyHelper;
  bool m_isInverted = false;
  std::atomic_bool m_disabled{false};
  DigitalOutput m_dio;
  PWM m_pwm;
  double m_speed = 0.0;
};

}  // namespace frc
