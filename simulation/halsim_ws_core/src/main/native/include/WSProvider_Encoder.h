// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "WSHalProviders.h"

namespace wpilibws {

class HALSimWSProviderEncoder : public HALSimWSHalChanProvider {
 public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderEncoder() override;

  void OnNetValueChanged(const wpi::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

 private:
  int32_t m_initCbKey = 0;
  int32_t m_countCbKey = 0;
  int32_t m_periodCbKey = 0;
  int32_t m_resetCbKey = 0;
  int32_t m_reverseDirectionCbKey = 0;
  int32_t m_samplesCbKey = 0;

  int32_t m_countOffset = 0;
};

}  // namespace wpilibws
