#ifndef NATIVE_FILEHANDLER_H
#define NATIVE_FILEHANDLER_H

#include "ChannelMap.h"
#include <string>

using namespace std;

const string CHANNEL_MAP_JSON = "mau/Translator/Maps/ChannelMap.json";
const string VMX_CHANNELS_JSON = "mau/Translator/Maps/VMXChannels.json";
const string VMX_ENUM_JSON = "mau/Translator/Maps/VMXEnum.json";

static VMXChannels createVMXChannels() {
    return NULL;
}

static ChannelMap createChannelMap(VMXChannels vmxChannels) {
    CHANNEL_MAP_JSON;
    return NULL;
}

#endif //NATIVE_FILEHANDLER_H
