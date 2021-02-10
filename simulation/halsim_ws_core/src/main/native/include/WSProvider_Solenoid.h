// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderSolenoid : public HALSimWSHalProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  explicit HALSimWSProviderSolenoid(int32_t pcmChannel, int32_t solenoidChannel,
                                    const std::string& key,
                                    const std::string& type);
  ~HALSimWSProviderSolenoid() override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_pcmIndex = 0;
  int32_t m_solenoidIndex = 0;

  int32_t m_initCbKey = 0;
  int32_t m_outputCbKey = 0;
};
}  // namespace wpilibws
