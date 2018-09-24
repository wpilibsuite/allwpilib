/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Interrupts.h>

#include "frc/AnalogTriggerType.h"
#include "frc/ErrorBase.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

class InterruptableSensorBase : public ErrorBase, public SendableBase {
 public:
  enum WaitResult {
    kTimeout = 0x0,
    kRisingEdge = 0x1,
    kFallingEdge = 0x100,
    kBoth = 0x101,
  };

  InterruptableSensorBase() = default;

  InterruptableSensorBase(InterruptableSensorBase&&) = default;
  InterruptableSensorBase& operator=(InterruptableSensorBase&&) = default;

  virtual HAL_Handle GetPortHandleForRouting() const = 0;
  virtual AnalogTriggerType GetAnalogTriggerTypeForRouting() const = 0;

  /**
   * Request one of the 8 interrupts asynchronously on this digital input.
   *
   * Request interrupts in asynchronous mode where the user's interrupt handler
   * will be called when the interrupt fires. Users that want control over the
   * thread priority should use the synchronous method with their own spawned
   * thread. The default is interrupt on rising edges only.
   */
  virtual void RequestInterrupts(HAL_InterruptHandlerFunction handler,
                                 void* param);

  /**
   * Request one of the 8 interrupts synchronously on this digital input.
   *
   * Request interrupts in synchronous mode where the user program will have to
   * explicitly wait for the interrupt to occur using WaitForInterrupt.
   * The default is interrupt on rising edges only.
   */
  virtual void RequestInterrupts();

  /**
   * Cancel interrupts on this device.
   *
   * This deallocates all the chipobject structures and disables any interrupts.
   */
  virtual void CancelInterrupts();

  /**
   * In synchronous mode, wait for the defined interrupt to occur.
   *
   * You should <b>NOT</b> attempt to read the sensor from another thread while
   * waiting for an interrupt. This is not threadsafe, and can cause memory
   * corruption
   *
   * @param timeout        Timeout in seconds
   * @param ignorePrevious If true, ignore interrupts that happened before
   *                       WaitForInterrupt was called.
   * @return What interrupts fired
   */
  virtual WaitResult WaitForInterrupt(double timeout,
                                      bool ignorePrevious = true);

  /**
   * Enable interrupts to occur on this input.
   *
   * Interrupts are disabled when the RequestInterrupt call is made. This gives
   * time to do the setup of the other options before starting to field
   * interrupts.
   */
  virtual void EnableInterrupts();

  /**
   * Disable Interrupts without without deallocating structures.
   */
  virtual void DisableInterrupts();

  /**
   * Return the timestamp for the rising interrupt that occurred most recently.
   *
   * This is in the same time domain as GetClock().
   * The rising-edge interrupt should be enabled with
   * {@link #DigitalInput.SetUpSourceEdge}
   *
   * @return Timestamp in seconds since boot.
   */
  virtual double ReadRisingTimestamp();

  /**
   * Return the timestamp for the falling interrupt that occurred most recently.
   *
   * This is in the same time domain as GetClock().
   * The falling-edge interrupt should be enabled with
   * {@link #DigitalInput.SetUpSourceEdge}
   *
   * @return Timestamp in seconds since boot.
   */
  virtual double ReadFallingTimestamp();

  /**
   * Set which edge to trigger interrupts on
   *
   * @param risingEdge  true to interrupt on rising edge
   * @param fallingEdge true to interrupt on falling edge
   */
  virtual void SetUpSourceEdge(bool risingEdge, bool fallingEdge);

 protected:
  HAL_InterruptHandle m_interrupt = HAL_kInvalidHandle;

  void AllocateInterrupts(bool watcher);
};

}  // namespace frc
