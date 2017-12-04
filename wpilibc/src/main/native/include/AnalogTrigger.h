/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <HAL/Types.h>

#include "AnalogTriggerOutput.h"
#include "SensorBase.h"

namespace frc {

class AnalogInput;

class AnalogTrigger : public SensorBase {
  friend class AnalogTriggerOutput;

 public:
  explicit AnalogTrigger(int channel);
  explicit AnalogTrigger(AnalogInput* channel);
  virtual ~AnalogTrigger();

  void SetLimitsVoltage(double lower, double upper);
  void SetLimitsRaw(int lower, int upper);
  void SetAveraged(bool useAveragedValue);
  void SetFiltered(bool useFilteredValue);
  int GetIndex() const;
  bool GetInWindow();
  bool GetTriggerState();
  std::shared_ptr<AnalogTriggerOutput> CreateOutput(
      AnalogTriggerType type) const;

 private:
  int m_index;
  HAL_AnalogTriggerHandle m_trigger;
  AnalogInput* m_analogInput = nullptr;
  bool m_ownsAnalog = false;
};

}  // namespace frc
