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
  ~HALSimWSProviderDriverStation();

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_enabledCbKey = 0;
  int32_t m_autonomousCbKey = 0;
  int32_t m_testCbKey = 0;
  int32_t m_estopCbKey = 0;
  int32_t m_fmsCbKey = 0;
  int32_t m_dsCbKey = 0;
  int32_t m_allianceCbKey = 0;
  int32_t m_matchTimeCbKey = 0;
  int32_t m_newDataCbKey = 0;
};

}  // namespace wpilibws
