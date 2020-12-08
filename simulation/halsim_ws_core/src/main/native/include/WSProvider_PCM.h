/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "WSHalProviders.h"

#define NUM_SOLENOIDS_PER_PCM 8

namespace wpilibws {

class HALSimWSProviderPCM : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderPCM();

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_solenoidInitCbKeys[NUM_SOLENOIDS_PER_PCM] = {0};
  int32_t m_solenoidOutputCbKeys[NUM_SOLENOIDS_PER_PCM] = {0};
  int32_t m_anySolenoidInitCbKey = 0;
  int32_t m_compressorInitCbKey = 0;
  int32_t m_compressorOnCbKey = 0;
  int32_t m_closedLoopEnabledCbKey = 0;
  int32_t m_pressureSwitchCbKey = 0;
  int32_t m_compressorCurrentCbKey = 0;
};

}  // namespace wpilibws
