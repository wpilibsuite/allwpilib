/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HAL/Ports.h>
#include <MockData/PWMData.h>

#include <NTProvider_PWM.h>

#include <cinttypes>

static void _PWMCallback(const char *name, void *param, const struct HAL_Value *value) {
    auto info = static_cast<struct NTProviderCallbackInfo *>(param);
    uint32_t chan = (uint32_t)info->channel;
    auto provider = static_cast<HALSimNTProviderPWM *>(info->provider);
    auto table = provider->table->GetSubTable(info->table_name);

    // Usually we would take the value directly from the callback function, but we're 
    // wanting to send back all values, not just speed/position/raw, but all at once,
    // so we either have to read all manually, or register multiple callbacks. In this case,
    // we're reading all manually to reduce overhead.
    table->GetEntry("init?").SetBoolean(HALSIM_GetPWMInitialized(chan) != 0);
    table->GetEntry("speed").SetDouble(HALSIM_GetPWMSpeed(chan));
    table->GetEntry("position").SetDouble(HALSIM_GetPWMPosition(chan));
    table->GetEntry("raw").SetDouble(HALSIM_GetPWMRawValue(chan));
    table->GetEntry("period_scale").SetDouble(HALSIM_GetPWMPeriodScale(chan));
    table->GetEntry("zero_latch?").SetBoolean(HALSIM_GetPWMZeroLatch(chan) != 0);
}

void HALSimNTProviderPWM::Initialize() {
    numChannels = HAL_GetNumPWMChannels();
    cbInfos.reserve(numChannels);

    for (int i = 0; i < numChannels; i++) {
        struct NTProviderCallbackInfo info = { this, "PWM" + std::to_string(i), i };
        cbInfos.push_back(info);
    }

    for (auto &info : cbInfos) {
        HALSIM_RegisterPWMAllCallbacks(info.channel, _PWMCallback, &info, true);
    }
}