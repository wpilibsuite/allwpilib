/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/Interrupts.h>

#include "AnalogTriggerType.h"
#include "SensorBase.h"

namespace frc {

class InterruptableSensorBase : public SensorBase {
 public:
  enum WaitResult {
    kTimeout = 0x0,
    kRisingEdge = 0x1,
    kFallingEdge = 0x100,
    kBoth = 0x101,
  };

  InterruptableSensorBase() = default;

  virtual HAL_Handle GetPortHandleForRouting() const = 0;
  virtual AnalogTriggerType GetAnalogTriggerTypeForRouting() const = 0;

  // Asynchronous handler version.
  virtual void RequestInterrupts(HAL_InterruptHandlerFunction handler,
                                 void* param);

  // Synchronous wait version.
  virtual void RequestInterrupts();

  // Free up the underlying ChipObject functions.
  virtual void CancelInterrupts();

  // Synchronous version.
  virtual WaitResult WaitForInterrupt(double timeout,
                                      bool ignorePrevious = true);

  // Enable interrupts - after finishing setup.
  virtual void EnableInterrupts();

  // Disable, but don't deallocate.
  virtual void DisableInterrupts();

  // Return the timestamp for the rising interrupt that occurred.
  virtual double ReadRisingTimestamp();

  // Return the timestamp for the falling interrupt that occurred.
  virtual double ReadFallingTimestamp();

  virtual void SetUpSourceEdge(bool risingEdge, bool fallingEdge);

 protected:
  HAL_InterruptHandle m_interrupt = HAL_kInvalidHandle;
  void AllocateInterrupts(bool watcher);
};

}  // namespace frc
