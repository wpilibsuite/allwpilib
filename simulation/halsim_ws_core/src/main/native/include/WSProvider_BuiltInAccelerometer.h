/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderBuiltInAccelerometer : public HALSimWSHalProvider {
 public:
  HALSimWSProviderBuiltInAccelerometer();
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;
  ~HALSimWSProviderBuiltInAccelerometer();

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_activeCbKey = 0;
  int32_t m_rangeCbKey = 0;
  int32_t m_xCbKey = 0;
  int32_t m_yCbKey = 0;
  int32_t m_zCbKey = 0;
};
}  // namespace wpilibws
