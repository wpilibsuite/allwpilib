// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HMB.h"

#include "FPGACalls.h"
#include "hal/ChipObject.h"
#include <memory>

using namespace hal;

static volatile uint32_t* hmbBuffer;
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
      reinterpret_cast<void**>(const_cast<uint32_t**>(&hmbBuffer)));

  if (*status != 0) {
    return;
  }

  auto cfg = hmb->readConfig(status);
  cfg.Enables_Timestamp = 1;
  cfg.Enables_DI = 1;
  cfg.Enables_PWM = 1;
  cfg.Enables_PWM_MXP = 1;
  cfg.Enables_AI0_Low = 1;
  cfg.Enables_AI0_High = 1;
  cfg.Enables_Counters_Low = 1;
  cfg.Enables_Counters_High = 1;
  hmb->writeConfig(cfg, status);
}

volatile uint32_t* HAL_GetHMBBuffer(void) {
  return hmbBuffer;
}
}
