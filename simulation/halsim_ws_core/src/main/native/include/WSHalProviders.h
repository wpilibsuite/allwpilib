// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include <hal/simulation/NotifyListener.h>
#include <wpi/json.h>
#include <wpi/mutex.h>

#include "WSBaseProvider.h"

namespace wpilibws {

using HALCbRegisterIndexedFunc = void (*)(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify);
using HALCbRegisterSingleFunc = void (*)(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);

// provider generates diffs based on values
class HALSimWSHalProvider : public HALSimWSBaseProvider {
 public:
  using HALSimWSBaseProvider::HALSimWSBaseProvider;

  void OnNetworkConnected(
      std::shared_ptr<HALSimBaseWebSocketConnection> ws) override;
  void OnNetworkDisconnected() override;

  void ProcessHalCallback(const wpi::json& payload);

 protected:
  virtual void RegisterCallbacks() = 0;
  virtual void CancelCallbacks() = 0;
};

// provider generates per-channel diffs
class HALSimWSHalChanProvider : public HALSimWSHalProvider {
 public:
  explicit HALSimWSHalChanProvider(int32_t channel, std::string_view key,
                                   std::string_view type);

  int32_t GetChannel() { return m_channel; }

 protected:
  int32_t m_channel;
};

using WSRegisterFunc = std::function<void(
    std::string_view, std::shared_ptr<HALSimWSBaseProvider>)>;

template <typename T>
void CreateProviders(std::string_view prefix, int32_t numChannels,
                     WSRegisterFunc webRegisterFunc);

template <typename T>
void CreateSingleProvider(std::string_view key, WSRegisterFunc webRegisterFunc);

}  // namespace wpilibws

#include "WSHalProviders.inc"
