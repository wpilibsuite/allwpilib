/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimLowFi.h"

#include <wpi/Twine.h>

void HALSimLowFi::Initialize() {
  table = nt::NetworkTableInstance::GetDefault().GetTable("sim");
}

void HALSimNTProvider::Inject(std::shared_ptr<HALSimLowFi> parentArg,
                              std::string tableNameArg) {
  parent = parentArg;
  tableName = std::move(tableNameArg);
  table = parent->table->GetSubTable(tableName);

  this->Initialize();
}

void NTProviderBaseCallback(const char* name, void* param,
                            const struct HAL_Value* value) {
  auto info =
      static_cast<struct HALSimNTProvider::NTProviderCallbackInfo*>(param);
  uint32_t chan = static_cast<uint32_t>(info->channel);
  auto provider = info->provider;
  auto table = info->table;
  provider->OnCallback(chan, table);
}

void HALSimNTProvider::InitializeDefault(
    int numChannels, HALCbRegisterIndexedFunc registerFunc) {
  this->numChannels = numChannels;
  cbInfos.reserve(numChannels);
  for (int i = 0; i < numChannels; i++) {
    struct NTProviderCallbackInfo info = {
        this, table->GetSubTable(tableName + wpi::Twine(i)), i};
    cbInfos.emplace_back(info);
  }

  for (auto& info : cbInfos) {
    registerFunc(info.channel, NTProviderBaseCallback, &info, true);
    OnInitializedChannel(info.channel, info.table);
  }
}

void HALSimNTProvider::InitializeDefaultSingle(
    HALCbRegisterSingleFunc registerFunc) {
  struct NTProviderCallbackInfo info = {this, table, 0};
  cbInfos.push_back(info);

  for (auto& info : cbInfos) {
    registerFunc(NTProviderBaseCallback, &info, true);
  }
}

void HALSimNTProvider::OnInitializedChannel(
    uint32_t channel, std::shared_ptr<nt::NetworkTable> table) {}
