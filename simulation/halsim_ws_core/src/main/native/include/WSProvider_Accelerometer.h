#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderAccelerometer : public HALSimWSHalChanProvider {
public:
  static void Initialize(WSRegisterFunc webRegisterFunc);

  using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
  ~HALSimWSProviderAccelerometer();

protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override;
  void DoCancelCallbacks();

private:
  int32_t m_activeCbKey = 0;
  int32_t m_rangeCbKey = 0;
  int32_t m_xCbKey = 0;
  int32_t m_yCbKey = 0;
  int32_t m_zCbKey = 0;
};
} // namespace wpilibws