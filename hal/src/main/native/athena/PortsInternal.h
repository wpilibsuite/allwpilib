// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/ChipObject.h"

namespace hal {

constexpr int32_t kNumAccumulators = tAccumulator::kNumSystems;
constexpr int32_t kNumAnalogTriggers = tAnalogTrigger::kNumSystems;
constexpr int32_t kNumAnalogInputs = 8;
constexpr int32_t kNumAnalogOutputs = tAO::kNumMXPRegisters;
constexpr int32_t kNumCounters = tCounter::kNumSystems;
constexpr int32_t kNumDigitalHeaders = 10;
constexpr int32_t kNumDigitalMXPChannels = 16;
constexpr int32_t kNumDigitalSPIPortChannels = 5;
constexpr int32_t kNumPWMHeaders = tPWM::kNumHdrRegisters;
constexpr int32_t kNumDigitalChannels =
    kNumDigitalHeaders + kNumDigitalMXPChannels + kNumDigitalSPIPortChannels;
constexpr int32_t kNumPWMChannels = tPWM::kNumMXPRegisters + kNumPWMHeaders;
constexpr int32_t kNumDigitalPWMOutputs =
    tDIO::kNumPWMDutyCycleAElements + tDIO::kNumPWMDutyCycleBElements;
constexpr int32_t kNumEncoders = tEncoder::kNumSystems;
constexpr int32_t kNumInterrupts = tInterrupt::kNumSystems;
constexpr int32_t kNumRelayChannels = 8;
constexpr int32_t kNumRelayHeaders = kNumRelayChannels / 2;
constexpr int32_t kNumCTREPCMModules = 63;
constexpr int32_t kNumCTRESolenoidChannels = 8;
constexpr int32_t kNumCTREPDPModules = 63;
constexpr int32_t kNumCTREPDPChannels = 16;
constexpr int32_t kNumREVPDHModules = 63;
constexpr int32_t kNumREVPDHChannels = 24;
constexpr int32_t kNumDutyCycles = tDutyCycle::kNumSystems;
constexpr int32_t kNumAddressableLEDs = tLED::kNumSystems;
constexpr int32_t kNumREVPHModules = 63;
constexpr int32_t kNumREVPHChannels = 16;

}  // namespace hal
