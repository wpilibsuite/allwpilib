// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderPDP : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderPDP();

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_initCbKey = 0;
  int32_t m_temperatureCbKey = 0;
  int32_t m_voltageCbKey = 0;
};

class HALSimWSProviderPDPChannelCurrent : public HALSimWSHalProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  explicit HALSimWSProviderPDPChannelCurrent(int32_t pdpChannel,
                                             int32_t currentChannel,
                                             const std::string& key,
                                             const std::string& type);
  ~HALSimWSProviderPDPChannelCurrent();

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_pdpIndex = 0;
  int32_t m_channelIndex = 0;

  int32_t m_currentCbKey = 0;
};
}  // namespace wpilibws
