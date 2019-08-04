/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

#include <mockdata/NotifyListener.h>
#include <networktables/NetworkTableInstance.h>

class HALSimLowFi {
 public:
  std::shared_ptr<nt::NetworkTable> table;
  void Initialize();
};

typedef void (*HALCbRegisterIndexedFunc)(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
typedef void (*HALCbRegisterSingleFunc)(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);

void NTProviderBaseCallback(const char* name, void* param,
                            const struct HAL_Value* value);

class HALSimNTProvider {
 public:
  struct NTProviderCallbackInfo {
    HALSimNTProvider* provider;
    std::shared_ptr<nt::NetworkTable> table;
    int channel;
  };

  void Inject(std::shared_ptr<HALSimLowFi> parent, std::string table);
  // Initialize is called by inject.
  virtual void Initialize() = 0;
  virtual void InitializeDefault(int numChannels,
                                 HALCbRegisterIndexedFunc registerFunc);
  virtual void InitializeDefaultSingle(HALCbRegisterSingleFunc registerFunc);
  virtual void OnCallback(uint32_t channel,
                          std::shared_ptr<nt::NetworkTable> table) = 0;
  virtual void OnInitializedChannel(uint32_t channel,
                                    std::shared_ptr<nt::NetworkTable> table);

  int numChannels;
  std::string tableName;

  std::shared_ptr<HALSimLowFi> parent;
  std::shared_ptr<nt::NetworkTable> table;
  std::vector<NTProviderCallbackInfo> cbInfos;
};
