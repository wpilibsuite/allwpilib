// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/InterruptableSensorBase.h"

#include "frc/Errors.h"

using namespace frc;

InterruptableSensorBase::~InterruptableSensorBase() {
  if (m_interrupt == HAL_kInvalidHandle) {
    return;
  }
  int32_t status = 0;
  HAL_CleanInterrupts(m_interrupt, &status);
  // Ignore status, as an invalid handle just needs to be ignored.
}

void InterruptableSensorBase::RequestInterrupts(
    HAL_InterruptHandlerFunction handler, void* param) {
  FRC_Assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(false);

  int32_t status = 0;
  HAL_RequestInterrupts(
      m_interrupt, GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(GetAnalogTriggerTypeForRouting()),
      &status);
  SetUpSourceEdge(true, false);
  HAL_AttachInterruptHandler(m_interrupt, handler, param, &status);
  FRC_CheckErrorStatus(status, "AttachInterruptHandler");
}

void InterruptableSensorBase::RequestInterrupts(InterruptEventHandler handler) {
  FRC_Assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(false);

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
  FRC_CheckErrorStatus(status, "AttachInterruptHandler");
}

void InterruptableSensorBase::RequestInterrupts() {
  FRC_Assert(m_interrupt == HAL_kInvalidHandle);
  AllocateInterrupts(true);

  int32_t status = 0;
  HAL_RequestInterrupts(
      m_interrupt, GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(GetAnalogTriggerTypeForRouting()),
      &status);
  FRC_CheckErrorStatus(status, "RequestInterrupts");
  SetUpSourceEdge(true, false);
}

void InterruptableSensorBase::CancelInterrupts() {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_CleanInterrupts(m_interrupt, &status);
  // Ignore status, as an invalid handle just needs to be ignored.
  m_interrupt = HAL_kInvalidHandle;
  m_interruptHandler = nullptr;
}

InterruptableSensorBase::WaitResult InterruptableSensorBase::WaitForInterrupt(
    double timeout, bool ignorePrevious) {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int result;

  result = HAL_WaitForInterrupt(m_interrupt, timeout, ignorePrevious, &status);
  FRC_CheckErrorStatus(status, "WaitForInterrupt");

  // Rising edge result is the interrupt bit set in the byte 0xFF
  // Falling edge result is the interrupt bit set in the byte 0xFF00
  // Set any bit set to be true for that edge, and AND the 2 results
  // together to match the existing enum for all interrupts
  int32_t rising = (result & 0xFF) ? 0x1 : 0x0;
  int32_t falling = ((result & 0xFF00) ? 0x0100 : 0x0);
  return static_cast<WaitResult>(falling | rising);
}

void InterruptableSensorBase::EnableInterrupts() {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_EnableInterrupts(m_interrupt, &status);
  FRC_CheckErrorStatus(status, "EnableInterrupts");
}

void InterruptableSensorBase::DisableInterrupts() {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  HAL_DisableInterrupts(m_interrupt, &status);
  FRC_CheckErrorStatus(status, "DisableInterrupts");
}

double InterruptableSensorBase::ReadRisingTimestamp() {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int64_t timestamp = HAL_ReadInterruptRisingTimestamp(m_interrupt, &status);
  FRC_CheckErrorStatus(status, "ReadRisingTimestamp");
  return timestamp * 1e-6;
}

double InterruptableSensorBase::ReadFallingTimestamp() {
  FRC_Assert(m_interrupt != HAL_kInvalidHandle);
  int32_t status = 0;
  int64_t timestamp = HAL_ReadInterruptFallingTimestamp(m_interrupt, &status);
  FRC_CheckErrorStatus(status, "ReadFallingTimestamp");
  return timestamp * 1e-6;
}

void InterruptableSensorBase::SetUpSourceEdge(bool risingEdge,
                                              bool fallingEdge) {
  if (m_interrupt == HAL_kInvalidHandle) {
    throw FRC_MakeError(
        err::NullParameter,
        "You must call RequestInterrupts before SetUpSourceEdge");
  }
  if (m_interrupt != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_SetInterruptUpSourceEdge(m_interrupt, risingEdge, fallingEdge, &status);
    FRC_CheckErrorStatus(status, "SetUpSourceEdge");
  }
}

void InterruptableSensorBase::AllocateInterrupts(bool watcher) {
  FRC_Assert(m_interrupt == HAL_kInvalidHandle);
  // Expects the calling leaf class to allocate an interrupt index.
  int32_t status = 0;
  m_interrupt = HAL_InitializeInterrupts(watcher, &status);
  FRC_CheckErrorStatus(status, "AllocateInterrupts");
}
