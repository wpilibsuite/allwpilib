#ifndef NATIVE_CHANNELMAP_H
#define NATIVE_CHANNELMAP_H

#include "HAL/handles/HandlesInternal.h"
#include "VMXChannels.h"
#include "FileHandler.h"
#include <map>

struct Mau_SingleChannel {
    int index;
    bool unsupported;

    bool isRef;
    Mau_SingleChannel* channelRef;

    bool isMXP;
    VMX_SingleChannel* vmxChannel;
};

class Mau_ChannelMap {
    Mau_SingleChannel channels [];
    int onBoardCount;
    int mxpCount;
public:
    friend static Mau_ChannelMap createChannelMap(VMX_Channels vmxChannels);

    Mau_SingleChannel getChannel(int index) {
        return channels[index];
    }
    int getChannelCount() {
        return (onBoardCount + mxpCount);
    }
    int getBoardCount() {
        return onBoardCount;
    }
    int getMXPCount() {
        return mxpCount;
    }
};

#endif //NATIVE_CHANNELMAP_H