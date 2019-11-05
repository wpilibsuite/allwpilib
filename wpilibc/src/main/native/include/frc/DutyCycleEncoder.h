/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/Types.h>

#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class DutyCycle;
class DigitalSource;

class DutyCycleEncoder : public ErrorBase, public Sendable, public SendableHelper<DutyCycleEncoder> {
 public:
  explicit DutyCycleEncoder(DutyCycle& dutyCycle);
  explicit DutyCycleEncoder(DutyCycle* dutyCycle);
  explicit DutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle);

  explicit DutyCycleEncoder(DigitalSource& digitalSource);
  explicit DutyCycleEncoder(DigitalSource* digitalSource);
  explicit DutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource);

  ~DutyCycleEncoder() override = default;

  DutyCycleEncoder(DutyCycleEncoder&&) = default;
  DutyCycleEncoder& operator=(DutyCycleEncoder&&) = default;

  void SetCountsPerRotation(int countsPerRotation);

  int GetFrequency() const;

  void Reset();

  int Get() const;
  int GetRotations() const;
  int GetPositionInRotation() const;

  void SetDistancePerPulse(double distancePerPulse);
  double GetDistancePerPulse(double distancePerPulse) const;
  double GetDistance() const;

  void SetSimDevice(HAL_SimDeviceHandle device);

  void InitSendable(SendableBuilder& builder) override;

 private:
  void Init();

  std::shared_ptr<DutyCycle> m_dutyCycle;
  AnalogTrigger m_analogTrigger;
  Counter m_counter;
  int m_countsPerRotation = 1024;
  int m_positionOffset = 0;
};
}
