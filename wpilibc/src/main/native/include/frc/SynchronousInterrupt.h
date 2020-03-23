/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/units.h>

#include "frc/ErrorBase.h"

namespace frc {
class DigitalSource;

class SynchronousInterrupt : public ErrorBase {
 public:
  enum WaitResult {
    kTimeout = 0x0,
    kRisingEdge = 0x1,
    kFallingEdge = 0x100,
    kBoth = 0x101,
  };

  explicit SynchronousInterrupt(DigitalSource& source);
  explicit SynchronousInterrupt(DigitalSource* source);
  explicit SynchronousInterrupt(std::shared_ptr<DigitalSource> source);

  ~SynchronousInterrupt() override;

  SynchronousInterrupt(SynchronousInterrupt&&) = default;
  SynchronousInterrupt& operator=(SynchronousInterrupt&&) = default;

  WaitResult WaitForInterrupt(units::second_t timeout,
                              bool ignorePrevious = false);
  void SetInterruptEdges(bool risingEdge, bool fallingEdge);

  units::second_t GetRisingTimestamp();
  units::second_t GetFallingTimestamp();

  void WakeupWaitingInterrupt();

 private:
  void InitSynchronousInterrupt();
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_InterruptHandle> m_handle;
};
}  // namespace frc
