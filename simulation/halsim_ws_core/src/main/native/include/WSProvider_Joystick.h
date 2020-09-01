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

class HALSimWSProviderJoystick : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderJoystick();

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_dsNewDataCbKey = 0;
};

}  // namespace wpilibws
