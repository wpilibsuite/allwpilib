/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class DigitalSource;
class AnalogTrigger;

class DutyCycle : public ErrorBase,
                  public Sendable,
                  public SendableHelper<DutyCycle> {
  friend class AnalogTrigger;

 public:
  explicit DutyCycle(DigitalSource& source);
  explicit DutyCycle(DigitalSource* source);
  explicit DutyCycle(std::shared_ptr<DigitalSource> source);

  ~DutyCycle() override;

  DutyCycle(DutyCycle&&) = default;
  DutyCycle& operator=(DutyCycle&&) = default;

  int GetFrequency() const;
  double GetOutput() const;
  unsigned int GetOutputRaw() const;
  unsigned int GetOutputScaleFactor() const;

  int GetFPGAIndex() const;

  void InitSendable(SendableBuilder& builder) override;

 private:
  void InitDutyCycle();
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_DutyCycleHandle> m_handle;
};
}  // namespace frc
