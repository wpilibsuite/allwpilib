#ifndef NATIVE_VMXCHANNELS_H
#define NATIVE_VMXCHANNELS_H

#include "FileHandler.h"
#include <VMXChannel.h>

struct VMX_SingleChannel {
    bool used;
    VMXChannelIndex index;
    VMXChannelType type;
    VMXChannelCapability capability;
};

struct VMX_Channels {
    friend static VMX_Channels createVMXChannels();

    VMX_SingleChannel getChannel(int index) {
        return channels[index];
    }
private:
    VMX_SingleChannel channels[64];
};

#endif //NATIVE_VMXCHANNELS_H
