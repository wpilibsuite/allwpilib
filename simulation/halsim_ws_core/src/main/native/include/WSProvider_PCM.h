/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderCompressor : public HALSimWSHalChanProvider {
 public:
  void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderCompressor();

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

class HALSimWSProviderSolenoid : public HALSimWSHalChanProvider {
 public:
  void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderSolenoid();

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  // TODO: Support multiple PCMs
  int32_t m_pcmIndex = 0;

  int32_t m_initCbKey = 0;
  int32_t m_outputCbKey = 0;
};
}  // namespace wpilibws
