/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "SafePWM.h"
#include "SpeedController.h"
#include "networktables/NetworkTableEntry.h"

namespace frc {

/**
 * Standard hobby style servo.
 *
 * The range parameters default to the appropriate values for the Hitec HS-322HD
 * servo provided
 * in the FIRST Kit of Parts in 2008.
 */
class Servo : public SafePWM {
 public:
  explicit Servo(int channel);
  virtual ~Servo();
  void Set(double value);
  void SetOffline();
  double Get() const;
  void SetAngle(double angle);
  double GetAngle() const;
  static double GetMaxAngle() { return kMaxServoAngle; }
  static double GetMinAngle() { return kMinServoAngle; }

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<nt::NetworkTable> subTable) override;

 protected:
  nt::NetworkTableEntry m_valueEntry;
  NT_EntryListener m_valueListener = 0;

 private:
  double GetServoAngleRange() const { return kMaxServoAngle - kMinServoAngle; }

  static constexpr double kMaxServoAngle = 180.0;
  static constexpr double kMinServoAngle = 0.0;

  static constexpr double kDefaultMaxServoPWM = 2.4;
  static constexpr double kDefaultMinServoPWM = .6;
};

}  // namespace frc
