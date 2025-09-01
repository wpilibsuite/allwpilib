

#ifndef __FRC_SYSTEMCORE__
#include <hal/Notifier.h>
#include <hal/handles/HandlesInternal.h>
#include <hal/simulation/NotifierData.h>

void HALSIM_ResetGlobalHandles() {

  // if we just reset the handles, notifiers might hang forever,
  // so we just enumerate and cancel them all
  auto sz = HALSIM_GetNotifierInfo(nullptr, 0);
  if (sz > 0) {
    struct HALSIM_NotifierInfo *info = new struct HALSIM_NotifierInfo[sz];
    HALSIM_GetNotifierInfo(info, sz);

    for (int i = 0; i < sz; i++) {
      HAL_CleanNotifier(info->handle);
    }
  }

  hal::HandleBase::ResetGlobalHandles();
}

#else

void HALSIM_ResetGlobalHandles() {}

#endif
