// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DMA.h>
#include <hal/DutyCycle.h>
#include <hal/HAL.h>

namespace hlt {

struct InterruptHandle {
 public:
  explicit InterruptHandle(int32_t* status) {
    handle = HAL_InitializeInterrupts(status);
  }
  InterruptHandle(const InterruptHandle&) = delete;
  InterruptHandle operator=(const InterruptHandle&) = delete;

  InterruptHandle(InterruptHandle&&) = default;
  InterruptHandle& operator=(InterruptHandle&&) = default;

  ~InterruptHandle() { HAL_CleanInterrupts(handle); }

  operator HAL_InterruptHandle() const { return handle; }

 private:
  HAL_InterruptHandle handle = 0;
};

struct DMAHandle {
 public:
  explicit DMAHandle(int32_t* status) { handle = HAL_InitializeDMA(status); }
  DMAHandle(const DMAHandle&) = delete;
  DMAHandle operator=(const DMAHandle&) = delete;

  DMAHandle(DMAHandle&&) = default;
  DMAHandle& operator=(DMAHandle&&) = default;

  ~DMAHandle() { HAL_FreeDMA(handle); }

  operator HAL_DMAHandle() const { return handle; }

 private:
  HAL_DMAHandle handle = 0;
};

struct AnalogInputHandle {
 public:
  AnalogInputHandle(int32_t port, int32_t* status) {
    handle = HAL_InitializeAnalogInputPort(HAL_GetPort(port), nullptr, status);
  }
  AnalogInputHandle(const AnalogInputHandle&) = delete;
  AnalogInputHandle operator=(const AnalogInputHandle&) = delete;

  AnalogInputHandle(AnalogInputHandle&&) = default;
  AnalogInputHandle& operator=(AnalogInputHandle&&) = default;

  ~AnalogInputHandle() { HAL_FreeAnalogInputPort(handle); }

  operator HAL_AnalogInputHandle() const { return handle; }

 private:
  HAL_AnalogInputHandle handle = 0;
};

struct AnalogOutputHandle {
 public:
  AnalogOutputHandle(int32_t port, int32_t* status) {
    handle = HAL_InitializeAnalogOutputPort(HAL_GetPort(port), nullptr, status);
  }
  AnalogOutputHandle(const AnalogOutputHandle&) = delete;
  AnalogOutputHandle operator=(const AnalogOutputHandle&) = delete;

  AnalogOutputHandle(AnalogOutputHandle&&) = default;
  AnalogOutputHandle& operator=(AnalogOutputHandle&&) = default;

  ~AnalogOutputHandle() { HAL_FreeAnalogOutputPort(handle); }

  operator HAL_AnalogOutputHandle() const { return handle; }

 private:
  HAL_AnalogOutputHandle handle = 0;
};

struct AnalogTriggerHandle {
 public:
  explicit AnalogTriggerHandle(HAL_AnalogInputHandle port, int32_t* status) {
    handle = HAL_InitializeAnalogTrigger(port, status);
  }
  AnalogTriggerHandle(const AnalogTriggerHandle&) = delete;
  AnalogTriggerHandle operator=(const AnalogTriggerHandle&) = delete;

  AnalogTriggerHandle(AnalogTriggerHandle&&) = default;
  AnalogTriggerHandle& operator=(AnalogTriggerHandle&&) = default;

  ~AnalogTriggerHandle() {
    int32_t status = 0;
    HAL_CleanAnalogTrigger(handle, &status);
  }

  operator HAL_AnalogTriggerHandle() const { return handle; }

 private:
  HAL_AnalogTriggerHandle handle = 0;
};

struct DutyCycleHandle {
 public:
  DutyCycleHandle(HAL_DigitalHandle port, int32_t* status) {
    handle = HAL_InitializeDutyCycle(
        port, HAL_AnalogTriggerType::HAL_Trigger_kInWindow, status);
  }
  DutyCycleHandle(const DutyCycleHandle&) = delete;
  DutyCycleHandle operator=(const DutyCycleHandle&) = delete;

  DutyCycleHandle(DutyCycleHandle&&) = default;
  DutyCycleHandle& operator=(DutyCycleHandle&&) = default;

  ~DutyCycleHandle() { HAL_FreeDutyCycle(handle); }

  operator HAL_DutyCycleHandle() const { return handle; }

 private:
  HAL_DutyCycleHandle handle = 0;
};

struct DIOHandle {
 public:
  DIOHandle() {}
  DIOHandle(const DIOHandle&) = delete;
  DIOHandle operator=(const DIOHandle&) = delete;

  DIOHandle(DIOHandle&&) = default;
  DIOHandle& operator=(DIOHandle&&) = default;

  DIOHandle(int32_t port, HAL_Bool input, int32_t* status) {
    handle = HAL_InitializeDIOPort(HAL_GetPort(port), input, nullptr, status);
  }

  ~DIOHandle() { HAL_FreeDIOPort(handle); }

  operator HAL_DigitalHandle() const { return handle; }

 private:
  HAL_DigitalHandle handle = 0;
};

struct PWMHandle {
 public:
  PWMHandle() {}
  PWMHandle(const PWMHandle&) = delete;
  PWMHandle operator=(const PWMHandle&) = delete;

  PWMHandle(PWMHandle&&) = default;
  PWMHandle& operator=(PWMHandle&&) = default;

  PWMHandle(int32_t port, int32_t* status) {
    handle = HAL_InitializePWMPort(HAL_GetPort(port), nullptr, status);
  }

  ~PWMHandle() {
    int32_t status = 0;
    HAL_FreePWMPort(handle, &status);
  }

  operator HAL_DigitalHandle() const { return handle; }

 private:
  HAL_DigitalHandle handle = 0;
};

struct RelayHandle {
 public:
  RelayHandle(int32_t port, HAL_Bool fwd, int32_t* status) {
    handle = HAL_InitializeRelayPort(HAL_GetPort(port), fwd, nullptr, status);
  }
  RelayHandle(const RelayHandle&) = delete;
  RelayHandle operator=(const RelayHandle&) = delete;

  RelayHandle(RelayHandle&&) = default;
  RelayHandle& operator=(RelayHandle&&) = default;

  ~RelayHandle() { HAL_FreeRelayPort(handle); }

  operator HAL_RelayHandle() const { return handle; }

 private:
  HAL_RelayHandle handle = 0;
};

#define ASSERT_LAST_ERROR_STATUS(status, x)                          \
  do {                                                               \
    ASSERT_EQ(status, HAL_USE_LAST_ERROR);                           \
    const char* lastErrorMessageInMacro = HAL_GetLastError(&status); \
    static_cast<void>(lastErrorMessageInMacro);                      \
    ASSERT_EQ(status, x);                                            \
  } while (0)

}  // namespace hlt
