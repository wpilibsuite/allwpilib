// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DMA.h>
#include <hal/DutyCycle.h>
#include <hal/HAL.h>

namespace hlt {

struct InterruptHandle : hal::Handle<HAL_InterruptHandle, HAL_CleanInterrupts> {
 public:
  explicit InterruptHandle(int32_t* status)
      : Handle{HAL_InitializeInterrupts(status)} {}
};

struct DMAHandle : hal::Handle<HAL_DMAHandle, HAL_FreeDMA> {
 public:
  explicit DMAHandle(int32_t* status) : Handle{HAL_InitializeDMA(status)} {}
};

struct AnalogInputHandle
    : hal::Handle<HAL_AnalogInputHandle, HAL_FreeAnalogInputPort> {
 public:
  AnalogInputHandle(int32_t port, int32_t* status)
      : Handle{HAL_InitializeAnalogInputPort(HAL_GetPort(port), nullptr,
                                             status)} {}
};

struct AnalogOutputHandle
    : hal::Handle<HAL_AnalogOutputHandle, HAL_FreeAnalogOutputPort> {
 public:
  AnalogOutputHandle(int32_t port, int32_t* status)
      : Handle{HAL_InitializeAnalogOutputPort(HAL_GetPort(port), nullptr,
                                              status)} {}
};

struct AnalogTriggerHandle
    : hal::Handle<HAL_AnalogTriggerHandle, HAL_CleanAnalogTrigger> {
 public:
  explicit AnalogTriggerHandle(HAL_AnalogInputHandle port, int32_t* status)
      : Handle{HAL_InitializeAnalogTrigger(port, status)} {}
};

struct DutyCycleHandle : hal::Handle<HAL_DutyCycleHandle, HAL_FreeDutyCycle> {
 public:
  DutyCycleHandle(HAL_DigitalHandle port, int32_t* status)
      : Handle{HAL_InitializeDutyCycle(
            port, HAL_AnalogTriggerType::HAL_Trigger_kInWindow, status)} {}
};

struct DIOHandle : hal::Handle<HAL_DigitalHandle, HAL_FreeDIOPort> {
 public:
  DIOHandle() {}

  DIOHandle(int32_t port, HAL_Bool input, int32_t* status)
      : Handle{
            HAL_InitializeDIOPort(HAL_GetPort(port), input, nullptr, status)} {}
};

struct PWMHandle : hal::Handle<HAL_DigitalHandle, HAL_FreePWMPort> {
 public:
  PWMHandle() {}

  PWMHandle(int32_t port, int32_t* status)
      : Handle{HAL_InitializePWMPort(HAL_GetPort(port), nullptr, status)} {}
};

struct RelayHandle : hal::Handle<HAL_RelayHandle, HAL_FreeRelayPort> {
 public:
  RelayHandle(int32_t port, HAL_Bool fwd, int32_t* status)
      : Handle{
            HAL_InitializeRelayPort(HAL_GetPort(port), fwd, nullptr, status)} {}
};

#define ASSERT_LAST_ERROR_STATUS(status, x)                          \
  do {                                                               \
    ASSERT_EQ(status, HAL_USE_LAST_ERROR);                           \
    [[maybe_unused]]                                                 \
    const char* lastErrorMessageInMacro = HAL_GetLastError(&status); \
    ASSERT_EQ(status, x);                                            \
  } while (0)

}  // namespace hlt
