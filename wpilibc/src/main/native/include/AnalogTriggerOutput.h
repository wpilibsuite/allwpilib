/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/AnalogTrigger.h>

#include "DigitalSource.h"

namespace frc {

class AnalogTrigger;

/**
 * Class to represent a specific output from an analog trigger.
 *
 * This class is used to get the current output value and also as a
 * DigitalSource to provide routing of an output to digital subsystems on the
 * FPGA such as Counter, Encoder, and Interrupt.
 *
 * The TriggerState output indicates the primary output value of the trigger.
 * If the analog signal is less than the lower limit, the output is false. If
 * the analog value is greater than the upper limit, then the output is true.
 * If the analog value is in between, then the trigger output state maintains
 * its most recent value.
 *
 * The InWindow output indicates whether or not the analog signal is inside the
 * range defined by the limits.
 *
 * The RisingPulse and FallingPulse outputs detect an instantaneous transition
 * from above the upper limit to below the lower limit, and vise versa. These
 * pulses represent a rollover condition of a sensor and can be routed to an up
 * / down counter or to interrupts. Because the outputs generate a pulse, they
 * cannot be read directly. To help ensure that a rollover condition is not
 * missed, there is an average rejection filter available that operates on the
 * upper 8 bits of a 12 bit number and selects the nearest outlyer of 3 samples.
 * This will reject a sample that is (due to averaging or sampling) errantly
 * between the two limits. This filter will fail if more than one sample in a
 * row is errantly in between the two limits. You may see this problem if
 * attempting to use this feature with a mechanical rollover sensor, such as a
 * 360 degree no-stop potentiometer without signal conditioning, because the
 * rollover transition is not sharp / clean enough. Using the averaging engine
 * may help with this, but rotational speeds of the sensor will then be limited.
 */
class AnalogTriggerOutput : public DigitalSource {
  friend class AnalogTrigger;

 public:
  virtual ~AnalogTriggerOutput();
  bool Get() const;

  // DigitalSource interface
  HAL_Handle GetPortHandleForRouting() const override;
  AnalogTriggerType GetAnalogTriggerTypeForRouting() const override;
  bool IsAnalogTrigger() const override;
  int GetChannel() const override;

 protected:
  AnalogTriggerOutput(const AnalogTrigger& trigger,
                      AnalogTriggerType outputType);

 private:
  // Uses reference rather than smart pointer because a user can not construct
  // an AnalogTriggerOutput themselves and because the AnalogTriggerOutput
  // should always be in scope at the same time as an AnalogTrigger.
  const AnalogTrigger& m_trigger;
  AnalogTriggerType m_outputType;
};

}  // namespace frc
