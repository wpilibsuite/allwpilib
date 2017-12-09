/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HAL/Ports.h>
#include <MockData/DIOData.h>

#include <NTProvider_DIO.h>

#include <cinttypes>

static void _DIOCallback(const char *name, void *param, const struct HAL_Value *value) {
    auto info = static_cast<struct NTProviderCallbackInfo *>(param);
    uint32_t chan = (uint32_t)info->channel;
    auto provider = static_cast<HALSimNTProviderDIO *>(info->provider);
    auto table = provider->table->GetSubTable(info->table_name);

    table->GetEntry("init?").SetBoolean(HALSIM_GetDIOInitialized(chan) != 0);
    table->GetEntry("value").SetBoolean(HALSIM_GetDIOValue(chan) != 0);
    table->GetEntry("pulse_length").SetDouble(HALSIM_GetDIOPulseLength(chan));
    table->GetEntry("input?").SetBoolean(HALSIM_GetDIOIsInput(chan) != 0);
}

void HALSimNTProviderDIO::Initialize() {
    numChannels = HAL_GetNumDigitalChannels();
    cbInfos.reserve(numChannels);

    for (int i = 0; i < numChannels; i++) {
        struct NTProviderCallbackInfo info = { this, "DIO" + std::to_string(i), i };
        cbInfos.push_back(info);
    }

    for (auto &info : cbInfos) {
        HALSIM_RegisterDIOAllCallbacks(info.channel, _DIOCallback, &info, true);
    }
}