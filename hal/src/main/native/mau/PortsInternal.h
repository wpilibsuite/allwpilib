/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

namespace hal {
    extern int32_t kNumAccumulators;
    extern int32_t kNumAnalogTriggers;
    extern int32_t kNumAnalogInputs;
    extern int32_t kNumAnalogOutputs;
    extern int32_t kNumCounters;
    extern int32_t kNumDigitalHeaders;
    extern int32_t kNumPWMHeaders;
    extern int32_t kNumDigitalChannels;
    extern int32_t kNumPWMChannels;
    extern int32_t kNumDigitalPWMOutputs;
    extern int32_t kNumEncoders;
    extern int32_t kNumInterrupts;
    extern int32_t kNumRelayChannels;
    extern int32_t kNumRelayHeaders;
    extern int32_t kNumPCMModules;
    extern int32_t kNumSolenoidChannels;
    extern int32_t kNumPDPModules;
    extern int32_t kNumPDPChannels;
    extern int32_t kNumCanTalons;
}
