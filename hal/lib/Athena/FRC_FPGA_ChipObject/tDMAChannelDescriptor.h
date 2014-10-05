// Describes the information needed to configure a DMA channel.
// Copyright (c) National Instruments 2008.  All Rights Reserved.

#include <stdint.h>

#ifndef __tDMAChannelDescriptor_h__
#define __tDMAChannelDescriptor_h__

struct tDMAChannelDescriptor
{
   uint32_t channel;
   uint32_t baseAddress;
   uint32_t depth;
   bool targetToHost;
};

#endif // __tDMAChannelDescriptor_h__
