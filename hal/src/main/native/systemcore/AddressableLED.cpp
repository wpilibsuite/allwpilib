// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AddressableLED.h"

#include <stdint.h>

#include <cstdio>
#include <cstring>
#include <thread>

#include <fmt/format.h>

#include "AddressableLEDSimd.hpp"
#include "HALInitializer.hpp"
#include "HALInternal.hpp"
#include "PortsInternal.hpp"
#include "SmartIo.hpp"
#include "SystemServerInternal.hpp"
#include "wpi/hal/AddressableLEDTypes.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/cpp/fpga_clock.hpp"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/RawTopic.hpp"

using namespace wpi::hal;

#define IO_PREFIX "/io/"

namespace {

constexpr const char* kLedsKey = IO_PREFIX "leds";

struct AddressableLEDs {
  explicit AddressableLEDs(wpi::nt::NetworkTableInstance inst)
      : rawPub{inst.GetRawTopic(kLedsKey).Publish(
            "raw", {.periodic = 0.005, .sendAll = true})} {}

  wpi::nt::RawPublisher rawPub;
  uint8_t s_buffer[HAL_kAddressableLEDMaxLength * 3];
};

static AddressableLEDs* leds;

void ConvertAndCopyLEDData(void* dst, const struct HAL_AddressableLEDData* src,
                           int32_t len, HAL_AddressableLEDColorOrder order) {
  using namespace wpi::hal::detail;
  switch (order) {
    case HAL_ALED_RGB:
      std::memcpy(dst, src, len * sizeof(HAL_AddressableLEDData));
      break;
    case HAL_ALED_RBG:
      ConvertPixels<HAL_ALED_RBG>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_BGR:
      ConvertPixels<HAL_ALED_BGR>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_BRG:
      ConvertPixels<HAL_ALED_BRG>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_GBR:
      ConvertPixels<HAL_ALED_GBR>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_GRB:
      ConvertPixels<HAL_ALED_GRB>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
  }
}
}  // namespace

namespace wpi::hal::init {
void InitializeAddressableLED() {
  static AddressableLEDs leds_static{wpi::hal::GetSystemServer()};
  leds = &leds_static;
}
}  // namespace wpi::hal::init

extern "C" {

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  wpi::hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    wpi::hal::SetLastErrorIndexOutOfRange(
        status, "Invalid Index for AddressableLED", 0, kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = smartIoHandles->Allocate(channel, HAL_HandleEnum::AddressableLED,
                                       &handle, status);

  if (*status != 0) {
    if (port) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                                port->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(
          status, "Invalid Index for AddressableLED", 0, kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::AddressableLED);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::AddressableLED);
    return HAL_kInvalidHandle;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(handle, HAL_HandleEnum::AddressableLED);

  // Wait for no other object to hold this handle.
  auto start = wpi::hal::fpga_clock::now();
  while (port.use_count() != 1) {
    auto current = wpi::hal::fpga_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("AddressableLED handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }
}

void HAL_SetAddressableLEDStart(HAL_AddressableLEDHandle handle, int32_t start,
                                int32_t* status) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = port->SetLedStart(start);
}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = port->SetLedLength(length);
}

static_assert(sizeof(HAL_AddressableLEDData) == 3, "LED Data must be 3 bytes");

void HAL_SetAddressableLEDData(int32_t start, int32_t length,
                               HAL_AddressableLEDColorOrder colorOrder,
                               const struct HAL_AddressableLEDData* data,
                               int32_t* status) {
  if (start < 0 || start >= HAL_kAddressableLEDMaxLength || length < 0 ||
      (start + length) >= HAL_kAddressableLEDMaxLength) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  ConvertAndCopyLEDData(&leds->s_buffer[start * 3], data, length, colorOrder);
  leds->rawPub.Set(leds->s_buffer);
}
}  // extern "C"
