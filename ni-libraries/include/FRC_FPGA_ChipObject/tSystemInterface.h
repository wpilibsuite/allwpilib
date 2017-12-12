// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __tSystemInterface_h__
#define __tSystemInterface_h__

#include "tDMAChannelDescriptor.h"

namespace nFPGA
{

class tSystemInterface
{
public:
   tSystemInterface(){}
   virtual ~tSystemInterface(){}

   virtual const uint16_t getExpectedFPGAVersion()=0;
   virtual const uint32_t getExpectedFPGARevision()=0;
   virtual const uint32_t * const getExpectedFPGASignature()=0;
   virtual void getHardwareFpgaSignature(uint32_t *guid_ptr, tRioStatusCode *status)=0;
   virtual uint32_t getLVHandle(tRioStatusCode *status)=0;
   virtual uint32_t getHandle()=0;
   virtual void reset(tRioStatusCode *status)=0;
   virtual void getDmaDescriptor(int dmaChannelDescriptorIndex, tDMAChannelDescriptor *desc)=0;
};

}

#endif // __tSystemInterface_h__

