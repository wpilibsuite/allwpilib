// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SynchronousInterrupt.h"

#include <type_traits>

#include <hal/Interrupts.h>

#include "frc/DigitalSource.h"
#include "frc/WPIErrors.h"

using namespace frc;

SynchronousInterrupt::SynchronousInterrupt(DigitalSource& source)
    : m_source{&source, NullDeleter<DigitalSource>()} {
  InitSynchronousInterrupt();
}
SynchronousInterrupt::SynchronousInterrupt(DigitalSource* source)
    : m_source{source, NullDeleter<DigitalSource>()} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitSynchronousInterrupt();
  }
}
SynchronousInterrupt::SynchronousInterrupt(
    std::shared_ptr<DigitalSource> source)
    : m_source{std::move(source)} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitSynchronousInterrupt();
  }
}

void SynchronousInterrupt::InitSynchronousInterrupt() {
  int32_t status = 0;
  m_handle = HAL_InitializeInterrupts(true, &status);
  wpi_setHALError(status);
  if (status != 0) {
    return;
  }
  HAL_RequestInterrupts(m_handle, m_source->GetPortHandleForRouting(),
                        static_cast<HAL_AnalogTriggerType>(
                            m_source->GetAnalogTriggerTypeForRouting()),
                        &status);
  wpi_setHALError(status);
  if (status != 0) {
    return;
  }
  HAL_SetInterruptUpSourceEdge(m_handle, true, false, &status);
  wpi_setHALError(status);
}

SynchronousInterrupt::~SynchronousInterrupt() {
  int32_t status = 0;
  HAL_CleanInterrupts(m_handle, &status);
}

inline SynchronousInterrupt::WaitResult operator|(
    SynchronousInterrupt::WaitResult lhs,
    SynchronousInterrupt::WaitResult rhs) {
  using T = std::underlying_type_t<SynchronousInterrupt::WaitResult>;
  return static_cast<SynchronousInterrupt::WaitResult>(static_cast<T>(lhs) |
                                                       static_cast<T>(rhs));
}

SynchronousInterrupt::WaitResult SynchronousInterrupt::WaitForInterrupt(
    units::second_t timeout, bool ignorePrevious) {
  if (StatusIsFatal())
    return WaitResult::kTimeout;
  int32_t status = 0;
  auto result = HAL_WaitForInterrupt(m_handle, timeout.to<double>(),
                                     ignorePrevious, &status);

  auto rising =
      ((result & 0xFF) != 0) ? WaitResult::kRisingEdge : WaitResult::kTimeout;
  auto falling = ((result & 0xFF00) != 0) ? WaitResult::kFallingEdge
                                          : WaitResult::kTimeout;

  return rising | falling;
}

void SynchronousInterrupt::SetInterruptEdges(bool risingEdge,
                                             bool fallingEdge) {
  if (StatusIsFatal())
    return;
  int32_t status = 0;
  HAL_SetInterruptUpSourceEdge(m_handle, risingEdge, fallingEdge, &status);
  wpi_setHALError(status);
}

void SynchronousInterrupt::WakeupWaitingInterrupt() {
  if (StatusIsFatal())
    return;
  int32_t status = 0;
  HAL_ReleaseWaitingInterrupt(m_handle, &status);
  wpi_setHALError(status);
}

units::second_t SynchronousInterrupt::GetRisingTimestamp() {
  if (StatusIsFatal())
    return 0_s;
  int32_t status = 0;
  auto ts = HAL_ReadInterruptRisingTimestamp(m_handle, &status);
  wpi_setHALError(status);
  units::microsecond_t ms{static_cast<double>(ts)};
  return ms;
}

units::second_t SynchronousInterrupt::GetFallingTimestamp() {
  if (StatusIsFatal())
    return 0_s;
  int32_t status = 0;
  auto ts = HAL_ReadInterruptFallingTimestamp(m_handle, &status);
  wpi_setHALError(status);
  units::microsecond_t ms{static_cast<double>(ts)};
  return ms;
}
