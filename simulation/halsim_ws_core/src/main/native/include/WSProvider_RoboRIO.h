// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "WSHalProviders.h"

namespace wpilibws {

class HALSimWSProviderRoboRIO : public HALSimWSHalProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;
  ~HALSimWSProviderRoboRIO() override;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_fpgaCbKey = 0;
  int32_t m_vinVoltageCbKey = 0;
  int32_t m_vinCurrentCbKey = 0;
  int32_t m_6vVoltageCbKey = 0;
  int32_t m_6vCurrentCbKey = 0;
  int32_t m_6vActiveCbKey = 0;
  int32_t m_6vFaultsCbKey = 0;
  int32_t m_5vVoltageCbKey = 0;
  int32_t m_5vCurrentCbKey = 0;
  int32_t m_5vActiveCbKey = 0;
  int32_t m_5vFaultsCbKey = 0;
  int32_t m_3v3VoltageCbKey = 0;
  int32_t m_3v3CurrentCbKey = 0;
  int32_t m_3v3ActiveCbKey = 0;
  int32_t m_3v3FaultsCbKey = 0;
};

}  // namespace wpilibws
