#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderCompressor : public HALSimWSHalChanProvider {
 public:
   void Initialize(WSRegisterFunc webRegisterFunc);

   using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
   ~HALSimWSProviderCompressor();

 protected:
   void RegisterCallbacks() override;
   void CancelCallbacks() override;
   void DoCancelCallbacks();
 private:
   int32_t m_initCbKey = 0;
   int32_t m_onCbKey = 0;
   int32_t m_closedLoopCbKey = 0;
   int32_t m_pressureSwitchCbKey = 0;
   int32_t m_currentCbKey = 0;
};

class HALSimWSProviderSolenoid : public HALSimWSHalChanProvider {
 public:
   void Initialize(WSRegisterFunc webRegisterFunc);

   using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
   ~HALSimWSProviderSolenoid();

 protected:
   void RegisterCallbacks() override;
   void CancelCallbacks() override;
   void DoCancelCallbacks();
 private:
  //TODO: Support multiple PCMs
  int32_t m_pcmIndex = 0;

  int32_t m_initCbKey = 0;
  int32_t m_outputCbKey = 0;
};
}