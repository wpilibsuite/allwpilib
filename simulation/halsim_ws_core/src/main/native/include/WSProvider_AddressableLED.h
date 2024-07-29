// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderAddressableLED : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderAddressableLED() override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_initCbKey = 0;
  int32_t m_outputPortCbKey = 0;
  int32_t m_lengthCbKey = 0;
  int32_t m_runningCbKey = 0;
  int32_t m_dataCbKey = 0;
};
}  // namespace wpilibws
