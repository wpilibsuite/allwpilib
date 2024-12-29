// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SynchronousInterrupt.h"

#include <memory>
#include <type_traits>
#include <utility>

#include <hal/Interrupts.h>
#include <wpi/NullDeleter.h>

#include "frc/DigitalSource.h"
#include "frc/Errors.h"

using namespace frc;

SynchronousInterrupt::SynchronousInterrupt(DigitalSource& source)
    : m_source{&source, wpi::NullDeleter<DigitalSource>()} {
  InitSynchronousInterrupt();
}
SynchronousInterrupt::SynchronousInterrupt(DigitalSource* source)
    : m_source{source, wpi::NullDeleter<DigitalSource>()} {
  if (m_source == nullptr) {
    FRC_CheckErrorStatus(frc::err::NullParameter, "Source is null");
  } else {
    InitSynchronousInterrupt();
  }
}
SynchronousInterrupt::SynchronousInterrupt(
    std::shared_ptr<DigitalSource> source)
    : m_source{std::move(source)} {
  if (m_source == nullptr) {
    FRC_CheckErrorStatus(frc::err::NullParameter, "Source is null");
  } else {
    InitSynchronousInterrupt();
  }
}

void SynchronousInterrupt::InitSynchronousInterrupt() {
  int32_t status = 0;
  m_handle = HAL_InitializeInterrupts(&status);
  FRC_CheckErrorStatus(status, "Interrupt failed to initialize");
  HAL_RequestInterrupts(m_handle, m_source->GetPortHandleForRouting(),
                        static_cast<HAL_AnalogTriggerType>(
                            m_source->GetAnalogTriggerTypeForRouting()),
                        &status);
  FRC_CheckErrorStatus(status, "Interrupt request failed");
  HAL_SetInterruptUpSourceEdge(m_handle, true, false, &status);
  FRC_CheckErrorStatus(status, "Interrupt setting up source edge failed");
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
  int32_t status = 0;
  auto result =
      HAL_WaitForInterrupt(m_handle, timeout.value(), ignorePrevious, &status);

  auto rising =
      ((result & 0xFF) != 0) ? WaitResult::kRisingEdge : WaitResult::kTimeout;
  auto falling = ((result & 0xFF00) != 0) ? WaitResult::kFallingEdge
                                          : WaitResult::kTimeout;

  return rising | falling;
}

void SynchronousInterrupt::SetInterruptEdges(bool risingEdge,
                                             bool fallingEdge) {
  int32_t status = 0;
  HAL_SetInterruptUpSourceEdge(m_handle, risingEdge, fallingEdge, &status);
  FRC_CheckErrorStatus(status, "Interrupt setting edges failed");
}

void SynchronousInterrupt::WakeupWaitingInterrupt() {
  int32_t status = 0;
  HAL_ReleaseWaitingInterrupt(m_handle, &status);
  FRC_CheckErrorStatus(status, "Interrupt wakeup failed");
}

units::second_t SynchronousInterrupt::GetRisingTimestamp() {
  int32_t status = 0;
  auto ts = HAL_ReadInterruptRisingTimestamp(m_handle, &status);
  FRC_CheckErrorStatus(status, "Interrupt rising timestamp failed");
  units::microsecond_t ms{static_cast<double>(ts)};
  return ms;
}

units::second_t SynchronousInterrupt::GetFallingTimestamp() {
  int32_t status = 0;
  auto ts = HAL_ReadInterruptFallingTimestamp(m_handle, &status);
  FRC_CheckErrorStatus(status, "Interrupt falling timestamp failed");
  units::microsecond_t ms{static_cast<double>(ts)};
  return ms;
}
