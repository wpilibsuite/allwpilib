// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderPCM : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderPCM() override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_initCbKey = 0;
  int32_t m_onCbKey = 0;
  int32_t m_closedLoopCbKey = 0;
  int32_t m_pressureSwitchCbKey = 0;
  int32_t m_currentCbKey = 0;
};
}  // namespace wpilibws
