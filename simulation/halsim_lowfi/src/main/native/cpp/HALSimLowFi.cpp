/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HALSimLowFi.h>

void HALSimLowFi::Initialize() {
    table = nt::NetworkTableInstance::GetDefault().GetTable("sim");
}

void HALSimNTProvider::Inject(std::shared_ptr<HALSimLowFi> parentArg, std::string tableNameArg) {
    parent = parentArg;
    tableName = tableNameArg;
    table = parent->table->GetSubTable(tableName);

    this->Initialize();
}

void NTProviderBaseCallback(const char *name, void *param, const struct HAL_Value *value) {
    auto info = static_cast<struct HALSimNTProvider::NTProviderCallbackInfo *>(param);
    uint32_t chan = (uint32_t)info->channel;
    auto provider = info->provider;
    auto table = provider->table->GetSubTable(info->table_name);
    provider->OnCallback(chan, table);
}

void HALSimNTProvider::InitializeDefault(int numChannels, HALCbRegisterFunc registerFunc) {
    this->numChannels = numChannels;
    cbInfos.reserve(numChannels);
    for (int i = 0; i < numChannels; i++) {
        struct NTProviderCallbackInfo info = { this, tableName + std::to_string(i), i };
        cbInfos.push_back(info);
    }

    for (auto &info : cbInfos) {
        registerFunc(info.channel, NTProviderBaseCallback, &info, true);
    }
}