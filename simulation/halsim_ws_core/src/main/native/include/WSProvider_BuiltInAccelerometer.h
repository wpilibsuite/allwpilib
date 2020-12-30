// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderBuiltInAccelerometer : public HALSimWSHalProvider {
 public:
  HALSimWSProviderBuiltInAccelerometer();
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;
  ~HALSimWSProviderBuiltInAccelerometer() override;

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
