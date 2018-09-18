/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  virtual HAL_Handle GetPortHandleForRouting() const = 0;
  virtual AnalogTriggerType GetAnalogTriggerTypeForRouting() const = 0;
  virtual bool IsAnalogTrigger() const = 0;
  virtual int GetChannel() const = 0;
};

}  // namespace frc
