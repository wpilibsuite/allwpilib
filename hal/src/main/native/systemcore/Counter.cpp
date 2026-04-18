// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Counter.h"

#include <cstdio>
#include <thread>

#include <fmt/format.h>

#include "HALInitializer.hpp"
#include "PortsInternal.hpp"
#include "SmartIo.hpp"
#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/monotonic_clock.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeCounter() {}
}  // namespace wpi::hal::init

extern "C" {

HAL_CounterHandle HAL_InitializeCounter(int channel, HAL_Bool risingEdge,
                                        const char* allocationLocation,
                                        int32_t* status) {
  wpi::hal::init::CheckInit();
  if (channel == INVALID_HANDLE_INDEX || channel >= kNumSmartIo) {
    *status = MakeErrorIndexOutOfRange(RESOURCE_OUT_OF_RANGE,
                                       "Invalid Index for Counter", 0,
                                       kNumSmartIo, channel);
    return HAL_INVALID_HANDLE;
  }

  auto resource =
      smartIoHandles->Allocate(channel, HAL_HandleEnum::COUNTER, "Counter");

  if (!resource) {
    *status = resource.error();
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.
  }

  auto [handle, port] = *resource;
  port->channel = channel;

  *status =
      port->InitializeMode(risingEdge ? SmartIoMode::SingleCounterRising
                                      : SmartIoMode::SingleCounterFalling);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::COUNTER);
    return HAL_INVALID_HANDLE;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreeCounter(HAL_CounterHandle counterHandle) {
  auto port = smartIoHandles->Get(counterHandle, HAL_HandleEnum::COUNTER);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(counterHandle, HAL_HandleEnum::COUNTER);

  // Wait for no other object to hold this handle.
  auto start = wpi::hal::monotonic_clock::now();
  while (port.use_count() != 1) {
    auto current = wpi::hal::monotonic_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("DIO handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }
}

void HAL_SetCounterEdgeConfiguration(HAL_CounterHandle counterHandle,
                                     HAL_Bool risingEdge, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  // TODO
  return;
}

int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  auto port = smartIoHandles->Get(counterHandle, HAL_HandleEnum::COUNTER);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  int32_t ret = 0;
  *status = port->GetCounter(&ret);
  return ret;
  return 0;
}

double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

void HAL_SetCounterDirection(HAL_CounterHandle counterHandle, HAL_Bool countUp,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

}  // extern "C"
