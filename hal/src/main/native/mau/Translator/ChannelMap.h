#ifndef NATIVE_CHANNELMAP_H
#define NATIVE_CHANNELMAP_H

#include "HAL/handles/HandlesInternal.h"
#include "VMXChannels.h"
#include "FileHandler.h"
#include <map>

struct WPISingleChannel {
    int index;
    bool unsupported;

    bool isRef;
    WPISingleChannel* channelRef;

    bool isMXP;
    VMXSingleChannel* vmxChannel;
};

struct ChannelMap {
    friend static ChannelMap createChannelMap(VMXChannels vmxChannels);

    WPISingleChannel getChannel(int index) {
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
private:
    WPISingleChannel channels [];
    int onBoardCount;
    int mxpCount;
};

#endif //NATIVE_CHANNELMAP_H