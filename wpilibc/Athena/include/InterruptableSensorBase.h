/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "SensorBase.h"
#include "Resource.h"

#include <memory>

class InterruptableSensorBase : public SensorBase {
 public:
  enum WaitResult {
    kTimeout = 0x0,
    kRisingEdge = 0x1,
    kFallingEdge = 0x100,
    kBoth = 0x101,
  };

  InterruptableSensorBase();
  virtual ~InterruptableSensorBase() = default;
  virtual uint32_t GetChannelForRouting() const = 0;
  virtual uint32_t GetModuleForRouting() const = 0;
  virtual bool GetAnalogTriggerForRouting() const = 0;
  virtual void RequestInterrupts(
      InterruptHandlerFunction handler,
      void *param);                  ///< Asynchronus handler version.
  virtual void RequestInterrupts();  ///< Synchronus Wait version.
  virtual void
  CancelInterrupts();  ///< Free up the underlying chipobject functions.
  virtual WaitResult WaitForInterrupt(
      float timeout, bool ignorePrevious = true);  ///< Synchronus version.
  virtual void
  EnableInterrupts();  ///< Enable interrupts - after finishing setup.
  virtual void DisableInterrupts();       ///< Disable, but don't deallocate.
  virtual double ReadRisingTimestamp();   ///< Return the timestamp for the
                                          ///rising interrupt that occurred.
  virtual double ReadFallingTimestamp();  ///< Return the timestamp for the
                                          ///falling interrupt that occurred.
  virtual void SetUpSourceEdge(bool risingEdge, bool fallingEdge);

 protected:
  void *m_interrupt = nullptr;
  uint32_t m_interruptIndex = std::numeric_limits<uint32_t>::max();
  void AllocateInterrupts(bool watcher);

  static std::unique_ptr<Resource> m_interrupts;
};
