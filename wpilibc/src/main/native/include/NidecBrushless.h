/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "DigitalOutput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "MotorSafety.h"
#include "MotorSafetyHelper.h"
#include "PWM.h"
#include "SpeedController.h"
#include "networktables/NetworkTableEntry.h"

namespace frc {

/**
 * Nidec Brushless Motor.
 */
class NidecBrushless : public SpeedController,
                       public MotorSafety,
                       public LiveWindowSendable {
 public:
  NidecBrushless(int pwmChannel, int dioChannel);
  ~NidecBrushless() = default;

  // SpeedController interface
  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

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
  void InitTable(std::shared_ptr<nt::NetworkTable> subtable) override;
  std::string GetSmartDashboardType() const override;

  // LiveWindowSendable interface
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;

 private:
  MotorSafetyHelper m_safetyHelper;
  bool m_isInverted = false;
  DigitalOutput m_dio;
  PWM m_pwm;
  double m_speed = 0.0;
  nt::NetworkTableEntry m_valueEntry;
  int m_valueListener;
};

}  // namespace frc
