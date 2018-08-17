#include "Translator/include/FileHandler.h"

const char* CHANNEL_MAP_JSON = "/usr/local/wpilib/lib/Maps/ChannelMap.json";
const char* VMX_CHANNELS_JSON = "/usr/local/wpilib/lib/Maps/VMXChannels.json";
const char* VMX_ENUM_JSON = "/usr/local/wpilib/lib/Maps/EnumPairs.json";

// -------- Generators -------- //

Mau_Channel* Mau_FileHandler::genGroupChannels(Mau_ChannelMap* mauMap, const char* labelVal, rapidjson::Document* doc) {
    bool allRef = (*doc)[labelVal]["Channels"].IsString();

    if (allRef) {
        std::string label(labelVal);
        mauMap->setChannelAsReference(label, (*doc)[labelVal]["Channels"].GetString());
        return nullptr;
    } else {
        int chanCount = (*doc)[labelVal]["Count"].GetInt();
        Mau_Channel* newChannels = new Mau_Channel[chanCount];

        int firstMXP = (*doc)[labelVal]["FirstMXP"].GetInt();
        std::string ref = (*doc)[labelVal]["ReferenceTo"].GetString();
        for (int count = 0; count < chanCount; count++) {
            Mau_Channel curChannel;
            curChannel.isMXP = (count >= firstMXP);

            rapidjson::Value curVal(std::to_string(count).c_str(), doc->GetAllocator());

            curChannel.wpiIndex = count;
            int refIndex = (*doc)[labelVal]["Channels"][curVal].GetInt();


            curChannel.unsupported = (refIndex == -1);

            if (!curChannel.unsupported) {
                if (ref == "VMXChannel") {
                	curChannel.vmxIndex = refIndex;
                }
            }

            newChannels[count] = curChannel;
        }

        return newChannels;
    }
}

void Mau_FileHandler::genHandleGroup(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc) {
    const char* labelVal = label.c_str();

    int handle = (*doc)[labelVal]["EnumValue"].GetInt();

    Mau_Channel* channels = genGroupChannels(mauMap, labelVal, doc);
    Mau_HandledGroup* group = new Mau_HandledGroup(channels, handle);

    mauMap->setGroup(label, group);
}

void Mau_FileHandler::genEncoderChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc) {
    Mau_Channel channel;
}

void Mau_FileHandler::genSPIChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc) {
    Mau_Channel channel;
}

void Mau_FileHandler::genI2CChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc) {
    Mau_Channel channel;
}

void Mau_FileHandler::genSerialChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc) {
    Mau_Channel channel;
}

// -------- Readers -------- //

Mau_EnumConverter* Mau_FileHandler::Mau_FileHandler::readEnums() {
    Mau_EnumConverter* enums = new Mau_EnumConverter();
    FILE* vmxJSON = fopen(VMX_ENUM_JSON, "r");

    char readBuffer[65536];

    rapidjson::FileReadStream vmxStream(vmxJSON, readBuffer, sizeof(readBuffer));
    rapidjson::Document vmxDoc;

    vmxDoc.ParseStream(vmxStream);
    assert(vmxDoc.IsObject());

    for(int handleCount = 0; handleCount < 20; handleCount++) {
        auto value = std::to_string(handleCount).c_str();
        rapidjson::Value indexVal(value, vmxDoc.GetAllocator());

        std::string label = vmxDoc["HAL_HandleEnum"][value].GetString();
        hal::HAL_HandleEnum handle = (hal::HAL_HandleEnum) handleCount;
        enums->setHandlePair(label, handle);
    }

    for (int typeCount = 0; typeCount <= 4; typeCount++) {
        auto value = std::to_string(typeCount).c_str();
        rapidjson::Value indexVal(value, vmxDoc.GetAllocator());

        std::string label = vmxDoc["VMXChannelType"][value].GetString();
        VMXChannelType type = (VMXChannelType) typeCount;
        enums->setTypePair(label, type);
    }

    for(int compCount = 0; compCount < 20; compCount++) {
        int compValue;
        if(compCount - 1 < 0) {
            compValue = 0;
        } else {
            compValue = pow(2.0, compCount - 1);
        }

        auto value = std::to_string(compCount).c_str();
        rapidjson::Value indexVal(value, vmxDoc.GetAllocator());

        std::string label = vmxDoc["VMXChannelCapability"][indexVal].GetString();
        VMXChannelCapability capability = (VMXChannelCapability) compValue;
        enums->setCapabilityPair(label, capability);
    }

    fclose(vmxJSON);
    return enums;
}

Mau_EnumConverter* Mau_FileHandler::getEnumConverter() {
    return enums;
}

Mau_ChannelMap* Mau_FileHandler::readChannelMap() {
	Mau_ChannelMap* channelMap = new Mau_ChannelMap();
    FILE* mauJson = fopen(CHANNEL_MAP_JSON, "r");
    char readBuffer[65536];

    rapidjson::FileReadStream vmxStream(mauJson, readBuffer, sizeof(readBuffer));
    rapidjson::Document mauDoc;

    mauDoc.ParseStream(vmxStream);
    assert(mauDoc.IsObject());

    for (auto it = allGenerators.begin(); it != allGenerators.end(); ++it) {
        rapidjson::Value label(it->first.c_str(), mauDoc.GetAllocator());

        genFuncs creator = allGenerators.at(it->first);
        creator(channelMap, it->first, &mauDoc);
    }

    fclose(mauJson);
    return channelMap;
}

Mau_FileHandler::Mau_FileHandler() {
    enums = readEnums();
}

Mau_FileHandler::~Mau_FileHandler() {
    delete enums;
}
