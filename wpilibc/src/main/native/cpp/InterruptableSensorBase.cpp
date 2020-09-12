/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/InterruptableSensorBase.h"

#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

InterruptableSensorBase::~InterruptableSensorBase() {
  if (m_interrupt == HAL_kInvalidHandle) return;
  int32_t status = 0;
  HAL_CleanInterrupts(m_interrupt, &status);
  // Ignore status, as an invalid handle just needs to be ignored.
}

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
  wpi_setHALError(status);
}

void InterruptableSensorBase::RequestInterrupts(InterruptEventHandler handler) {
  if (StatusIsFatal()) return;

  wpi_assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(false);
  if (StatusIsFatal()) return;  // if allocate failed, out of interrupts

  m_interruptHandler =
      std::make_unique<InterruptEventHandler>(std::move(handler));

  int32_t status = 0;
  HAL_RequestInterrupts(
      m_interrupt, GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(GetAnalogTriggerTypeForRouting()),
      &status);
  SetUpSourceEdge(true, false);
  HAL_AttachInterruptHandler(
      m_interrupt,
      [](uint32_t mask, void* param) {
        auto self = reinterpret_cast<InterruptEventHandler*>(param);
        // Rising edge result is the interrupt bit set in the byte 0xFF
        // Falling edge result is the interrupt bit set in the byte 0xFF00
        // Set any bit set to be true for that edge, and AND the 2 results
        // together to match the existing enum for all interrupts
        int32_t rising = (mask & 0xFF) ? 0x1 : 0x0;
        int32_t falling = ((mask & 0xFF00) ? 0x0100 : 0x0);
        WaitResult res = static_cast<WaitResult>(falling | rising);
        (*self)(res);
      },
      m_interruptHandler.get(), &status);
  wpi_setHALError(status);
}

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
  wpi_setHALError(status);
  SetUpSourceEdge(true, false);
}

void InterruptableSensorBase::CancelInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_CleanInterrupts(m_interrupt, &status);
  // Ignore status, as an invalid handle just needs to be ignored.
  m_interrupt = HAL_kInvalidHandle;
  m_interruptHandler = nullptr;
}

InterruptableSensorBase::WaitResult InterruptableSensorBase::WaitForInterrupt(
    double timeout, bool ignorePrevious) {
  if (StatusIsFatal()) return InterruptableSensorBase::kTimeout;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int result;

  result = HAL_WaitForInterrupt(m_interrupt, timeout, ignorePrevious, &status);
  wpi_setHALError(status);

  // Rising edge result is the interrupt bit set in the byte 0xFF
  // Falling edge result is the interrupt bit set in the byte 0xFF00
  // Set any bit set to be true for that edge, and AND the 2 results
  // together to match the existing enum for all interrupts
  int32_t rising = (result & 0xFF) ? 0x1 : 0x0;
  int32_t falling = ((result & 0xFF00) ? 0x0100 : 0x0);
  return static_cast<WaitResult>(falling | rising);
}

void InterruptableSensorBase::EnableInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_EnableInterrupts(m_interrupt, &status);
  wpi_setHALError(status);
}

void InterruptableSensorBase::DisableInterrupts() {
  if (StatusIsFatal()) return;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_DisableInterrupts(m_interrupt, &status);
  wpi_setHALError(status);
}

double InterruptableSensorBase::ReadRisingTimestamp() {
  if (StatusIsFatal()) return 0.0;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int64_t timestamp = HAL_ReadInterruptRisingTimestamp(m_interrupt, &status);
  wpi_setHALError(status);
  return timestamp * 1e-6;
}

double InterruptableSensorBase::ReadFallingTimestamp() {
  if (StatusIsFatal()) return 0.0;
  wpi_assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int64_t timestamp = HAL_ReadInterruptFallingTimestamp(m_interrupt, &status);
  wpi_setHALError(status);
  return timestamp * 1e-6;
}

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
    wpi_setHALError(status);
  }
}

void InterruptableSensorBase::AllocateInterrupts(bool watcher) {
  wpi_assert(m_interrupt == HAL_kInvalidHandle);
  // Expects the calling leaf class to allocate an interrupt index.
  int32_t status = 0;
  m_interrupt = HAL_InitializeInterrupts(watcher, &status);
  wpi_setHALError(status);
}
