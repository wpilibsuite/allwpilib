/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "WSHalProviders.h"

namespace wpilibws {

class HALSimWSProviderDriverStation : public HALSimWSHalProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;

 private:
  int32_t m_enabledCbKey;
  int32_t m_autonomousCbKey;
  int32_t m_testCbKey;
  int32_t m_estopCbKey;
  int32_t m_fmsCbKey;
  int32_t m_dsCbKey;
  int32_t m_allianceCbKey;
  int32_t m_matchTimeCbKey;
};

}  // namespace wpilibws
