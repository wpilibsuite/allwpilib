#ifndef NATIVE_FILEHANDLER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "FileHandlerInternal.h"
#include "MauMap.h"
#include "MauEnumConverter.h"
#include <VMXChannel.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>

#define NATIVE_FILEHANDLER_H

class Mau_FileHandler {
    Mau_EnumConverter enums;

    static Mau_Channel* genGroupChannels(Mau_ChannelMap* mauMap, const char* labelVal, rapidjson::Document* doc);
    static void genHandleGroup(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);
    static void genEncoderChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);
    static void genSPIChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);
    static void genI2CChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);
    static void genSerialChannel(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);

    Mau_EnumConverter readEnums();
    Mau_VMXChannelRef readVMXChannels();

    typedef void(* genFuncs)(Mau_ChannelMap* mauMap, std::string label, rapidjson::Document* doc);
    const std::map<std::string, genFuncs> allGenerators = {
        {"DIO", &genHandleGroup},
        {"Interrupt", &genHandleGroup},
        {"AnalogOutput", &genHandleGroup},
        {"AnalogInput", &genHandleGroup},
        {"AnalogTrigger", &genHandleGroup},
        {"Relay", &genHandleGroup},
        {"PWM", &genHandleGroup},
        {"DigitalPWM", &genHandleGroup},
        {"Counter", &genHandleGroup},
        {"FPGAEncoder", &genEncoderChannel},
        {"Encoder", &genEncoderChannel},
        {"SPI", &genSPIChannel},
        {"I2C", &genI2CChannel},
        {"Serial", &genSerialChannel}
    };

public:
    Mau_ChannelMap readChannelMap();
    Mau_EnumConverter getEnumConverter();
    Mau_FileHandler();

};


#endif //NATIVE_FILEHANDLER_H
