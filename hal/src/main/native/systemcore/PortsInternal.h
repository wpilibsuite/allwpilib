// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {

constexpr int32_t kNumAccumulators = 0;
constexpr int32_t kNumAnalogTriggers = 0;
constexpr int32_t kNumAnalogInputs = 8;
constexpr int32_t kNumAnalogOutputs = 0;
constexpr int32_t kNumCounters = 0;
constexpr int32_t kNumDigitalHeaders = 10;
constexpr int32_t kNumDigitalMXPChannels = 16;
constexpr int32_t kNumDigitalSPIPortChannels = 5;
constexpr int32_t kNumPWMHeaders = 0;
constexpr int32_t kNumDigitalChannels =
    kNumDigitalHeaders + kNumDigitalMXPChannels + kNumDigitalSPIPortChannels;
constexpr int32_t kNumPWMChannels = 0 + kNumPWMHeaders;
constexpr int32_t kNumDigitalPWMOutputs = 0;
constexpr int32_t kNumEncoders = 0;
constexpr int32_t kNumInterrupts = 0;
constexpr int32_t kNumRelayChannels = 8;
constexpr int32_t kNumRelayHeaders = kNumRelayChannels / 2;
constexpr int32_t kNumCTREPCMModules = 63;
constexpr int32_t kNumCTRESolenoidChannels = 8;
constexpr int32_t kNumCTREPDPModules = 63;
constexpr int32_t kNumCTREPDPChannels = 16;
constexpr int32_t kNumREVPDHModules = 63;
constexpr int32_t kNumREVPDHChannels = 24;
constexpr int32_t kNumDutyCycles = 0;
constexpr int32_t kNumAddressableLEDs = 0;
constexpr int32_t kNumREVPHModules = 63;
constexpr int32_t kNumREVPHChannels = 16;

}  // namespace hal
