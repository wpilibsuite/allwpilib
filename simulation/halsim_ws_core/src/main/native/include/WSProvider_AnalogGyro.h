#pragma once

#include "WSHalProviders.h"

namespace wpilibws {
class HALSimWSProviderAnalogGyro : public HALSimWSHalChanProvider {
 public: 
   static void Initialize(WSRegisterFunc webRegisterFunc);

   using HALSimWSHalChanProvider::HALSimWSHalChanProvider;
   ~HALSimWSProviderAnalogGyro();

 protected:
   void RegisterCallbacks() override;
   void CancelCallbacks() override;
   void DoCancelCallbacks();
 private:
   int32_t m_angleCbKey = 0;
   int32_t m_rateCbKey = 0;
   int32_t m_initCbKey = 0;
};
}