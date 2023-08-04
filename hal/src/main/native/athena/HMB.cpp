// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/roborio/HMB.h"

#include <memory>

#include "FPGACalls.h"
#include "hal/ChipObject.h"

using namespace hal;

// 16 classes of data, each takes up 16 uint16_t's
static_assert(sizeof(HAL_HMBData) == 0x10 * 16 * sizeof(uint32_t));
// Timestamp is the last class, and should be offset correctly
static_assert(offsetof(HAL_HMBData, Timestamp) == 0x10 * 15 * sizeof(uint32_t));

static volatile HAL_HMBData* hmbBuffer;
static size_t hmbBufferSize;
static constexpr const char hmbName[] = "HMB_0_RAM";
static std::unique_ptr<tHMB> hmb;

namespace {
struct HMBHolder {
  ~HMBHolder() {
    if (hmbBuffer) {
      hal::HAL_NiFpga_CloseHmb(hmb->getSystemInterface()->getHandle(), hmbName);
    }
  }
};
}  // namespace

extern "C" {

void HAL_InitializeHMB(int32_t* status) {
  static HMBHolder holder;

  hmb.reset(tHMB::create(status));
  if (*status != 0) {
    return;
  }

  *status = hal::HAL_NiFpga_OpenHmb(
      hmb->getSystemInterface()->getHandle(), hmbName, &hmbBufferSize,
      reinterpret_cast<void**>(const_cast<HAL_HMBData**>(&hmbBuffer)));

  if (*status != 0) {
    return;
  }

  auto cfg = hmb->readConfig(status);
  cfg.Enables_AI0_Low = 1;
  cfg.Enables_AI0_High = 1;
  cfg.Enables_AIAveraged0_Low = 1;
  cfg.Enables_AIAveraged0_High = 1;
  cfg.Enables_Accumulator0 = 1;
  cfg.Enables_Accumulator1 = 1;
  cfg.Enables_DI = 1;
  cfg.Enables_AnalogTriggers = 1;
  cfg.Enables_Counters_Low = 1;
  cfg.Enables_Counters_High = 1;
  cfg.Enables_CounterTimers_Low = 1;
  cfg.Enables_CounterTimers_High = 1;
  cfg.Enables_Encoders_Low = 1;
  cfg.Enables_Encoders_High = 1;
  cfg.Enables_EncoderTimers_Low = 1;
  cfg.Enables_EncoderTimers_High = 1;
  cfg.Enables_DutyCycle_Low = 1;
  cfg.Enables_DutyCycle_High = 1;
  cfg.Enables_Interrupts = 1;
  cfg.Enables_PWM = 1;
  cfg.Enables_PWM_MXP = 1;
  cfg.Enables_Relay_DO_AO = 1;
  cfg.Enables_Timestamp = 1;
  hmb->writeConfig(cfg, status);
}

const volatile HAL_HMBData* HAL_GetHMBBuffer(void) {
  return hmbBuffer;
}
}  // extern "C"
