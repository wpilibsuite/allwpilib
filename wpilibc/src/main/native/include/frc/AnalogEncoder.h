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
class AnalogInput;

class AnalogEncoder : public ErrorBase, public Sendable, public SendableHelper<AnalogEncoder> {
 public:
  explicit AnalogEncoder(AnalogInput& analogInput);
  explicit AnalogEncoder(AnalogInput* analogInput);
  explicit AnalogEncoder(std::shared_ptr<AnalogInput> analogInput);

  ~AnalogEncoder() override = default;

  AnalogEncoder(AnalogEncoder&&) = default;
  AnalogEncoder& operator=(AnalogEncoder&&) = default;

  void Reset();

  double Get() const;
  int GetRotations() const;
  double GetPositionInRotation() const;

  double GetPositionOffset() const;

  void SetDistancePerRotation(double distancePerRotation);
  double GetDistancePerRotation() const;
  double GetDistance() const;

  void InitSendable(SendableBuilder& builder) override;

 private:
  void Init();

  std::shared_ptr<AnalogInput> m_analogInput;
  AnalogTrigger m_analogTrigger;
  Counter m_counter;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
};
}
