// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/halsim/ws_core/WSHalProviders.hpp"

namespace wpilibws {

class HALSimWSProviderAnalogIn : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderAnalogIn() override;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() final;
  void DoCancelCallbacks();

 private:
  int32_t m_initCbKey = 0;
  int32_t m_avgbitsCbKey = 0;
  int32_t m_oversampleCbKey = 0;
  int32_t m_voltageCbKey = 0;
};

}  // namespace wpilibws
