/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "SpeedController.h"

#include <memory>

/**
 * Standard hobby style servo.
 *
 * The range parameters default to the appropriate values for the Hitec HS-322HD
 * servo provided
 * in the FIRST Kit of Parts in 2008.
 */
class Servo : public SafePWM {
 public:
  explicit Servo(uint32_t channel);
  virtual ~Servo();
  void Set(float value);
  void SetOffline();
  float Get() const;
  void SetAngle(float angle);
  float GetAngle() const;
  static float GetMaxAngle() { return kMaxServoAngle; }
  static float GetMinAngle() { return kMinServoAngle; }

  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  std::shared_ptr<ITable> m_table;

 private:
  float GetServoAngleRange() const { return kMaxServoAngle - kMinServoAngle; }

  static constexpr float kMaxServoAngle = 180.0;
  static constexpr float kMinServoAngle = 0.0;

  static constexpr float kDefaultMaxServoPWM = 2.4;
  static constexpr float kDefaultMinServoPWM = .6;
};
