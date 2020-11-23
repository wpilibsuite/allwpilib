/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderPDP : public HALSimWSHalProvider {
 public:
  void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;
  ~HALSimWSProviderPDP();

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_index = 0;

  int32_t m_initCbKey = 0;
  int32_t m_temperatureCbKey = 0;
  int32_t m_voltageCbKey = 0;
  // int32_t m_currentCbKey = 0;
};
}  // namespace wpilibws
