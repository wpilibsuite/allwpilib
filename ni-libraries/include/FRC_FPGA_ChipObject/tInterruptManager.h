// Class for handling interrupts.
// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __tInterruptManager_h__
#define __tInterruptManager_h__

#include "tSystem.h"

namespace ni
{
   namespace dsc
   {
      namespace osdep
      {
         class CriticalSection;
      }
   }
}

namespace nFPGA
{

typedef void (*tInterruptHandler)(uint32_t interruptAssertedMask, void *param);

class tInterruptManager : public tSystem
{
public:
   tInterruptManager(uint32_t interruptMask, bool watcher, tRioStatusCode *status);
   ~tInterruptManager();
   void registerHandler(tInterruptHandler handler, void *param, tRioStatusCode *status);
   uint32_t watch(int32_t timeoutInMs, bool ignorePrevious, tRioStatusCode *status);
   void enable(tRioStatusCode *status);
   void disable(tRioStatusCode *status);
   bool isEnabled(tRioStatusCode *status);
private:
   class tInterruptThread;
   friend class tInterruptThread;
   void handler();
   static int handlerWrapper(tInterruptManager *pInterrupt);

   void acknowledge(tRioStatusCode *status);
   void reserve(tRioStatusCode *status);
   void unreserve(tRioStatusCode *status);
   tInterruptHandler _handler;
   uint32_t _interruptMask;
   tInterruptThread *_thread;
   NiFpga_IrqContext _rioContext;
   bool _watcher;
   bool _enabled;
   void *_userParam;

   // maintain the interrupts that are already dealt with.
   static uint32_t _globalInterruptMask;
   static ni::dsc::osdep::CriticalSection *_globalInterruptMaskSemaphore;
};

}


#endif // __tInterruptManager_h__

