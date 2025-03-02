// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {

constexpr int32_t kNumCanBuses = 2;
constexpr int32_t kNumSmartIo = 5;
constexpr int32_t kNumCTREPCMModules = 63;
constexpr int32_t kNumCTRESolenoidChannels = 8;
constexpr int32_t kNumCTREPDPModules = 63;
constexpr int32_t kNumCTREPDPChannels = 16;
constexpr int32_t kNumREVPDHModules = 63;
constexpr int32_t kNumREVPDHChannels = 24;
constexpr int32_t kNumAddressableLEDs = 0;
constexpr int32_t kNumREVPHModules = 63;
constexpr int32_t kNumREVPHChannels = 16;

}  // namespace hal
