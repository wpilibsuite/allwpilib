#ifndef NATIVE_CHANNELMAP_H
#define NATIVE_CHANNELMAP_H

#include "HandlesInternal.h"
#include "Translator/include/MauEnumConverter.h"
#include <VMXChannel.h>
#include <VMXResourceConfig.h>
#include <vector>
#include <string>
#include <map>

struct Mau_Channel {
    int wpiIndex;
    bool unsupported;
    bool isMXP;

    VMXChannelIndex vmxIndex;
    VMXChannelCapability vmxAbility;

    VMXResourceHandle vmxResHandle;
    VMXResourceConfig* vmxResConfig;

    VMXChannelInfo getInfo();
    VMXResourceIndex getResourceIndex();
};

class Mau_ChannelGroup {
    Mau_Channel* channels;
public:
    Mau_ChannelGroup(Mau_Channel* newChannels);
    Mau_Channel* getChannel(int index);
};

class Mau_HandledGroup : public Mau_ChannelGroup {
    hal::HAL_HandleEnum WPI_Handle;

public:
    Mau_HandledGroup(Mau_Channel* newChannels, int handle);
    hal::HAL_HandleEnum getHandle();
};

class Mau_ChannelMap {
    std::map<std::string, Mau_ChannelGroup*> groups;

    friend class Mau_FileHandler;
    void setChannelAsReference(std::string target, std::string ref);
    void setGroup(std::string target, Mau_ChannelGroup* group);
public:
    Mau_ChannelMap();
    Mau_Channel* getChannel(std::string label, int index);
    VMXChannelInfo getChannelInfo(std::string label, int index);

    void initializeChannel(std::string label, int index, VMXChannelCapability ability, VMXResourceConfig* config);
};

#endif //NATIVE_CHANNELMAP_H
