// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {
constexpr int32_t kNumCanBuses = 5;
constexpr int32_t kNumSmartIo = 6;
constexpr int32_t kNumI2cBuses = 2;
constexpr int32_t kNumEncoders = 3;
constexpr int32_t kNumAddressableLEDs = 1;
constexpr int32_t kNumCTREPCMModules = 63;
constexpr int32_t kNumCTRESolenoidChannels = 8;
constexpr int32_t kNumCTREPDPModules = 63;
constexpr int32_t kNumCTREPDPChannels = 16;
constexpr int32_t kNumREVPDHModules = 63;
constexpr int32_t kNumREVPDHChannels = 24;
constexpr int32_t kNumREVPHModules = 63;
constexpr int32_t kNumREVPHChannels = 16;

}  // namespace hal
