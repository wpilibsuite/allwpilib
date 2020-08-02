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

class HALSimWSProviderAnalogIn : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;

 private:
  int32_t m_initCbKey;
  int32_t m_avgbitsCbKey;
  int32_t m_oversampleCbKey;
  int32_t m_voltageCbKey;
  int32_t m_accumInitCbKey;
  int32_t m_accumValueCbKey;
  int32_t m_accumCountCbKey;
  int32_t m_accumCenterCbKey;
  int32_t m_accumDeadbandCbKey;
};

class HALSimWSProviderAnalogOut : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;

 private:
  int32_t m_initCbKey;
  int32_t m_voltageCbKey;
};

}  // namespace wpilibws
