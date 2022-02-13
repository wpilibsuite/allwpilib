// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {
constexpr int32_t kAccelerometers = 1;
constexpr int32_t kNumAccumulators = 2;
constexpr int32_t kNumAnalogTriggers = 8;
constexpr int32_t kNumAnalogInputs = 8;
constexpr int32_t kNumAnalogOutputs = 2;
constexpr int32_t kNumCounters = 8;
constexpr int32_t kNumDigitalHeaders = 10;
constexpr int32_t kNumPWMHeaders = 10;
constexpr int32_t kNumDigitalChannels = 31;
constexpr int32_t kNumPWMChannels = 20;
constexpr int32_t kNumDigitalPWMOutputs = 6;
constexpr int32_t kNumEncoders = 8;
constexpr int32_t kI2CPorts = 2;
constexpr int32_t kNumInterrupts = 8;
constexpr int32_t kNumRelayChannels = 8;
constexpr int32_t kNumRelayHeaders = kNumRelayChannels / 2;
constexpr int32_t kNumCTREPCMModules = 63;
constexpr int32_t kNumCTRESolenoidChannels = 8;
constexpr int32_t kNumCTREPDPModules = 63;
constexpr int32_t kNumCTREPDPChannels = 16;
constexpr int32_t kNumREVPDHModules = 63;
constexpr int32_t kNumREVPDHChannels = 24;
constexpr int32_t kNumPDSimModules = kNumREVPDHModules;
constexpr int32_t kNumPDSimChannels = kNumREVPDHChannels;
constexpr int32_t kNumDutyCycles = 8;
constexpr int32_t kNumAddressableLEDs = 1;
constexpr int32_t kNumREVPHModules = 63;
constexpr int32_t kNumREVPHChannels = 16;
constexpr int32_t kSPIAccelerometers = 5;
constexpr int32_t kSPIPorts = 5;

}  // namespace hal
