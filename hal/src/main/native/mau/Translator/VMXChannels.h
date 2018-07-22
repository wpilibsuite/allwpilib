#ifndef NATIVE_VMXCHANNELS_H
#define NATIVE_VMXCHANNELS_H

#include "FileHandler.h"
#include <VMXChannel.h>

struct VMXSingleChannel {
    bool used;
    VMXChannelIndex index;
    VMXChannelType type;
    VMXChannelCapability capability;
};

struct VMXChannels {
    friend static VMXChannels createVMXChannels();

    VMXSingleChannel getChannel(int index) {
        return channels[index];
    }
private:
    VMXSingleChannel channels[64];
};

#endif //NATIVE_VMXCHANNELS_H
