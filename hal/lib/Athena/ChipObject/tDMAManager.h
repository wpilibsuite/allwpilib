// Class for handling DMA transters.
// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __tDMAManager_h__
#define __tDMAManager_h__

#include "tSystem.h"

namespace nFPGA
{
// TODO: Implement DMA Manager
/*
class tDMAManager : public tSystem
{
public:
   tDMAManager(tNIRIO_u32 dmaChannel, tNIRIO_u32 hostBufferSize, tRioStatusCode *status);
   ~tDMAManager();
   void start(tRioStatusCode *status);
   void stop(tRioStatusCode *status);
   bool isStarted() {return _started;}
   void read(
      tNIRIO_u32*      buf,
      tNIRIO_u32       num,
      tNIRIO_u32       timeout,
      tNIRIO_u32*      read,
      tNIRIO_u32*      remaining,
      tRioStatusCode *status);
   void write(
      tNIRIO_u32*      buf,
      tNIRIO_u32       num,
      tNIRIO_u32       timeout,
      tNIRIO_u32*      remaining,
      tRioStatusCode *status);
private:
   bool _started;
   tNIRIO_u32 _dmaChannel;
   tNIRIO_u32 _hostBufferSize;
   tDMAChannelDescriptor const *_dmaChannelDescriptor;

};
*/
}


#endif // __tDMAManager_h__

