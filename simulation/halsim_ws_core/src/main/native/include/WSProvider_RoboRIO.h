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

class HALSimWSProviderRoboRIO : public HALSimWSHalProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;

 private:
  int32_t m_fpgaCbKey;
  int32_t m_vinVoltageCbKey;
  int32_t m_vinCurrentCbKey;
  int32_t m_6vVoltageCbKey;
  int32_t m_6vCurrentCbKey;
  int32_t m_6vActiveCbKey;
  int32_t m_6vFaultsCbKey;
  int32_t m_5vVoltageCbKey;
  int32_t m_5vCurrentCbKey;
  int32_t m_5vActiveCbKey;
  int32_t m_5vFaultsCbKey;
  int32_t m_3v3VoltageCbKey;
  int32_t m_3v3CurrentCbKey;
  int32_t m_3v3ActiveCbKey;
  int32_t m_3v3FaultsCbKey;
};

}  // namespace wpilibws
