/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "InterruptableSensorBase.h"

#include "HAL/HAL.h"
#include "Utility.h"
#include "WPIErrors.h"

using namespace frc;

InterruptableSensorBase::InterruptableSensorBase() {}

/**
 * Request one of the 8 interrupts asynchronously on this digital input.
 *
 * Request interrupts in asynchronous mode where the user's interrupt handler
 * will be called when the interrupt fires. Users that want control over the
 * thread priority should use the synchronous method with their own spawned
 * thread. The default is interrupt on rising edges only.
 */
void InterruptableSensorBase::RequestInterrupts(
    HAL_InterruptHandlerFunction handler, void* param) {
  if (StatusIsFatal()) return;

  wpi_assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(false);
  if (StatusIsFatal()) return;  // if allocate failed, out of interrupts

  int32_t status = 0;
  HAL_RequestInterrupts(
      m_interrupt, GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(GetAnalogTriggerTypeForRouting()),
      &status);
  SetUpSourceEdge(true, false);
  HAL_AttachInterruptHandler(m_interrupt, handler, param, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Request one of the 8 interrupts synchronously on this digital input.
 *
 * Request interrupts in synchronous mode where the user program will have to
 * explicitly wait for the interrupt to occur using WaitForInterrupt.
 * The default is interrupt on rising edges only.
 */
void InterruptableSensorBase::RequestInterrupts() {
  if (StatusIsFatal()) return;

  wpi_assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(true);
  if (StatusIsFatal()) return;  // if allocate failed, out of interrupts

  int32_t status = 0;
  HAL_RequestInterrupts(
      m_interrupt, GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(GetAnalogTriggerTypeForRouting()),
      &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  SetUpSourceEdge(true, false);
}

void InterruptableSensorBase::AllocateInterrupts(bool watcher) {
  wpi_assert(m_interrupt == HAL_kInvalidHandle);
  // Expects the calling leaf class to allocate an interrupt index.
  int32_t status = 0;
  m_interrupt = HAL_InitializeInterrupts(watcher, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Cancel interrupts on this device.
 *
 * This deallocates all the chipobject structures and disables any interrupts.
 */
void InterruptableSensorBase::CancelInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_CleanInterrupts(m_interrupt, &status);
  // ignore status, as an invalid handle just needs to be ignored.
  m_interrupt = HAL_kInvalidHandle;
}

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
InterruptableSensorBase::WaitResult InterruptableSensorBase::WaitForInterrupt(
    double timeout, bool ignorePrevious) {
  if (StatusIsFatal()) return InterruptableSensorBase::kTimeout;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int result;

  result = HAL_WaitForInterrupt(m_interrupt, timeout, ignorePrevious, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  // Rising edge result is the interrupt bit set in the byte 0xFF
  // Falling edge result is the interrupt bit set in the byte 0xFF00
  // Set any bit set to be true for that edge, and AND the 2 results
  // together to match the existing enum for all interrupts
  int32_t rising = (result & 0xFF) ? 0x1 : 0x0;
  int32_t falling = ((result & 0xFF00) ? 0x0100 : 0x0);
  return static_cast<WaitResult>(falling | rising);
}

/**
 * Enable interrupts to occur on this input.
 *
 * Interrupts are disabled when the RequestInterrupt call is made. This gives
 * time to do the setup of the other options before starting to field
 * interrupts.
 */
void InterruptableSensorBase::EnableInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_EnableInterrupts(m_interrupt, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Disable Interrupts without without deallocating structures.
 */
void InterruptableSensorBase::DisableInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_DisableInterrupts(m_interrupt, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Return the timestamp for the rising interrupt that occurred most recently.
 *
 * This is in the same time domain as GetClock().
 * The rising-edge interrupt should be enabled with
 * {@link #DigitalInput.SetUpSourceEdge}
 *
 * @return Timestamp in seconds since boot.
 */
double InterruptableSensorBase::ReadRisingTimestamp() {
  if (StatusIsFatal()) return 0.0;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  double timestamp = HAL_ReadInterruptRisingTimestamp(m_interrupt, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return timestamp;
}

/**
 * Return the timestamp for the falling interrupt that occurred most recently.
 *
 * This is in the same time domain as GetClock().
 * The falling-edge interrupt should be enabled with
 * {@link #DigitalInput.SetUpSourceEdge}
 *
 * @return Timestamp in seconds since boot.
*/
double InterruptableSensorBase::ReadFallingTimestamp() {
  if (StatusIsFatal()) return 0.0;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  double timestamp = HAL_ReadInterruptFallingTimestamp(m_interrupt, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return timestamp;
}

/**
 * Set which edge to trigger interrupts on
 *
 * @param risingEdge  true to interrupt on rising edge
 * @param fallingEdge true to interrupt on falling edge
 */
void InterruptableSensorBase::SetUpSourceEdge(bool risingEdge,
                                              bool fallingEdge) {
  if (StatusIsFatal()) return;
  if (m_interrupt == HAL_kInvalidHandle) {
    wpi_setWPIErrorWithContext(
        NullParameter,
        "You must call RequestInterrupts before SetUpSourceEdge");
    return;
  }
  if (m_interrupt != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_SetInterruptUpSourceEdge(m_interrupt, risingEdge, fallingEdge, &status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  }
}
