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
  ~HALSimWSProviderRoboRIO();

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
