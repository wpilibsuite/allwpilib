/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
constexpr int32_t kNumPCMModules = 63;
constexpr int32_t kNumSolenoidChannels = 8;
constexpr int32_t kNumPDPModules = 63;
constexpr int32_t kNumPDPChannels = 16;

}  // namespace hal
