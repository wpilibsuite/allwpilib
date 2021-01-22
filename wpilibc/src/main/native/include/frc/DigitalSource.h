// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>

#include "frc/InterruptableSensorBase.h"

namespace frc {

/**
 * DigitalSource Interface.
 *
 * The DigitalSource represents all the possible inputs for a counter or a
 * quadrature encoder. The source may be either a digital input or an analog
 * input. If the caller just provides a channel, then a digital input will be
 * constructed and freed when finished for the source. The source can either be
 * a digital input or analog trigger but not both.
 */
class DigitalSource : public InterruptableSensorBase {
 public:
  DigitalSource() = default;
  DigitalSource(DigitalSource&&) = default;
  DigitalSource& operator=(DigitalSource&&) = default;

  HAL_Handle GetPortHandleForRouting() const override = 0;
  AnalogTriggerType GetAnalogTriggerTypeForRouting() const override = 0;
  virtual bool IsAnalogTrigger() const = 0;
  virtual int GetChannel() const = 0;
};

}  // namespace frc
