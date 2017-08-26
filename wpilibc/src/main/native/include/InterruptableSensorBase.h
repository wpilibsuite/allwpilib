/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
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
  virtual ~InterruptableSensorBase() = default;

  virtual HAL_Handle GetPortHandleForRouting() const = 0;
  virtual AnalogTriggerType GetAnalogTriggerTypeForRouting() const = 0;
  virtual void RequestInterrupts(
      HAL_InterruptHandlerFunction handler,
      void* param);                  ///< Asynchronus handler version.
  virtual void RequestInterrupts();  ///< Synchronus Wait version.
  virtual void
  CancelInterrupts();  ///< Free up the underlying chipobject functions.
  virtual WaitResult WaitForInterrupt(
      double timeout,
      bool ignorePrevious = true);  ///< Synchronus version.
  virtual void
  EnableInterrupts();  ///< Enable interrupts - after finishing setup.
  virtual void DisableInterrupts();       ///< Disable, but don't deallocate.
  virtual double ReadRisingTimestamp();   ///< Return the timestamp for the
                                          /// rising interrupt that occurred.
  virtual double ReadFallingTimestamp();  ///< Return the timestamp for the
                                          /// falling interrupt that occurred.
  virtual void SetUpSourceEdge(bool risingEdge, bool fallingEdge);

 protected:
  HAL_InterruptHandle m_interrupt = HAL_kInvalidHandle;
  void AllocateInterrupts(bool watcher);
};

}  // namespace frc
