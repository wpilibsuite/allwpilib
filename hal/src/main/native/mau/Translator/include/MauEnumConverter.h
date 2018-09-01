#ifndef NATIVE_MAUENUMSINTERNAL_H

#include "HandlesInternal.h"
#include <VMXChannel.h>
#include <map>
#include <math.h>
#include <string>
#include <vector>
#include<utility>

#define NATIVE_MAUENUMSINTERNAL_H

class Mau_EnumConverter {
    std::vector<std::pair<std::string, hal::HAL_HandleEnum>> halHandles;
    std::vector<std::pair<std::string, VMXChannelCapability>> vmxCapabilities;
    std::vector<std::pair<std::string, VMXChannelType>> vmxTypes;
public:
    void setHandlePair(std::string label, hal::HAL_HandleEnum handle);
    void setCapabilityPair(std::string label, VMXChannelCapability capability);
    void setTypePair(std::string label, VMXChannelType type);

    std::string getHandleLabel(hal::HAL_HandleEnum handle);
    std::string getCapabilityLabel(VMXChannelCapability capability);
    std::string getTypeLabel(VMXChannelType type);

    hal::HAL_HandleEnum getHandleValue(std::string label);
    VMXChannelCapability getCapabilityValue(std::string label);
    VMXChannelType getTypeValue(std::string label);
};

#endif //NATIVE_MAUENUMSINTERNAL_H