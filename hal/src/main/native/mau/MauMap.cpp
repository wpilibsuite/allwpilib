#include "MauMap.h"

// ----- Mau Channel ----- //

VMXChannelInfo Mau_Channel::getInfo() {
    return VMXChannelInfo(vmxIndex, vmxAbility);
}

VMXResourceIndex Mau_Channel::getResourceIndex() {
    return EXTRACT_VMX_RESOURCE_INDEX(*vmxResHandle);
}

// ----- Hal Channel Group ----- //

Mau_ChannelGroup::Mau_ChannelGroup(Mau_Channel* newChannels) {
    channels = newChannels;
}

Mau_Channel* Mau_ChannelGroup::getChannel(int index) {
    return &channels[index];
}

// ----- Hal Channel Group: w/WPI_Handle ----- //

Mau_HandledGroup::Mau_HandledGroup(Mau_Channel* newChannels, int handle) : Mau_ChannelGroup(newChannels) {
    WPI_Handle = (hal::HAL_HandleEnum) handle;
}

hal::HAL_HandleEnum Mau_HandledGroup::Mau_HandledGroup::getHandle() {
    return WPI_Handle;
}

// ----- Mau Channel Map ----- //

Mau_ChannelMap::Mau_ChannelMap() {
    groups["DIO"] = NULL;
    groups["Interrupt"] = NULL;
    groups["AnalogOutput"] = NULL;
    groups["AnalogInput"] = NULL;
    groups["AnalogTrigger"] = NULL;
    groups["Relay"] = NULL;
    groups["PWM"] = NULL;
    groups["DigitalPWM"] = NULL;
    groups["Counter"] = NULL;
    groups["FPGAEncoder"] = NULL;
    groups["Encoder"] = NULL;
    groups["SPI"] = NULL;
    groups["I2C"] = NULL;
    groups["Serial"] = NULL;
}

// ----- Mau Map: Friend Creators ----- //

void Mau_ChannelMap::setChannelAsReference(std::string target, std::string ref) {
    groups[target] = groups[ref];
}

void Mau_ChannelMap::setGroup(std::string target, Mau_ChannelGroup* group) {
    groups[target] = group;
}

// ----- Mau Map: Getters ----- //

Mau_Channel* Mau_ChannelMap::getChannel(std::string label, int index) {
    return groups[label]->getChannel(index);
}

VMXChannelInfo Mau_ChannelMap::getChannelInfo(std::string label, int index) {
    Mau_Channel* channel = getChannel(label, index);
    return VMXChannelInfo(channel->vmxIndex, channel->vmxAbility);
}

// ----- Mau Map: Setters ----- //

void Mau_ChannelMap::initializeChannel(std::string label, int index, VMXChannelCapability ability, VMXResourceConfig* config) {
    Mau_Channel* channel = getChannel(label, index);
    channel->vmxResConfig = config;
    channel->vmxAbility = ability;
}
