
#include <VMXChannel.h>
#include "HandlesInternal.h"
#include "Translator/include/MauEnumConverter.h"
#include <utility>

void Mau_EnumConverter::setHandlePair(std::string label, hal::HAL_HandleEnum handle) {
    std::pair<std::string, hal::HAL_HandleEnum> newHandle;
    newHandle.first = label;
    newHandle.second = handle;
    halHandles.push_back(newHandle);
}

void Mau_EnumConverter::setCapabilityPair(std::string label, VMXChannelCapability capability) {
    std::pair<std::string, VMXChannelCapability> newCapability;
    newCapability.first = label;
    newCapability.second = capability;
    vmxCapabilities.push_back(newCapability);
}

void Mau_EnumConverter::setTypePair(std::string label, VMXChannelType type) {
    std::pair<std::string, VMXChannelType> newType;
    newType.first = label;
    newType.second = type;
    vmxTypes.push_back(newType);
}

std::string Mau_EnumConverter::getHandleLabel(hal::HAL_HandleEnum handle) {
    for(std::pair<std::string, hal::HAL_HandleEnum> curPair : halHandles) {
        if(curPair.second == handle) {
            return curPair.first;
        }
    }
    return "Not Found!";
}

std::string Mau_EnumConverter::getCapabilityLabel(VMXChannelCapability capability) {
    for(std::pair<std::string, VMXChannelCapability> curPair : vmxCapabilities) {
        if(curPair.second == capability) {
            return curPair.first;
        }
    }
    return "Not Found!";
}

std::string Mau_EnumConverter::getTypeLabel(VMXChannelType type) {
    for(std::pair<std::string, VMXChannelType> curPair : vmxTypes) {
        if(curPair.second == type) {
            return curPair.first;
        }
    }
    return "Not Found!";
}

hal::HAL_HandleEnum Mau_EnumConverter::getHandleValue(std::string label) {
    for(std::pair<std::string, hal::HAL_HandleEnum> curPair : halHandles) {
        if(curPair.first == label) {
            return curPair.second;
        }
    }
    return (hal::HAL_HandleEnum)0;
}

VMXChannelCapability Mau_EnumConverter::getCapabilityValue(std::string label) {
    for(std::pair<std::string, VMXChannelCapability> curPair : vmxCapabilities) {
        if(curPair.first == label) {
            return curPair.second;
        }
    }
    return (VMXChannelCapability)0;
}

VMXChannelType Mau_EnumConverter::getTypeValue(std::string label) {
    for(std::pair<std::string, VMXChannelType> curPair : vmxTypes) {
        if(curPair.first == label) {
            return curPair.second;
        }
    }
    return (VMXChannelType)0;
}