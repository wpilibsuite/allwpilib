// Class for handling DMA transfers.
// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __tDMAManager_h__
#define __tDMAManager_h__

#include "tSystem.h"
#include <stdint.h>

namespace nFPGA
{
class tDMAManager : public tSystem
{
public:
   tDMAManager(uint32_t dmaChannel, uint32_t hostBufferSize, tRioStatusCode *status);
   ~tDMAManager();
   void start(tRioStatusCode *status);
   void stop(tRioStatusCode *status);
   bool isStarted() {return _started;}
   void read(
      uint32_t*      buf,
      size_t         num,
      uint32_t       timeout,
      size_t*        remaining,
      tRioStatusCode *status);
   void write(
      uint32_t*      buf,
      size_t         num,
      uint32_t       timeout,
      size_t*        remaining,
      tRioStatusCode *status);
private:
   bool _started;
   uint32_t _dmaChannel;
   uint32_t _hostBufferSize;

};

}

#endif // __tDMAManager_h__
