#include "LogModelViewLoader.h"
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>

using namespace sapphire;

void LogModelViewLoader::Register(std::string_view typeName, CreateModelFunc createModel,
                CreateViewFunc createView){
    // m_typeMap[typeName] = Builder{std::move(createModel), std::move(createView)};
}

void LogModelViewLoader::AddDataLog(DataLogModel& model){
    // for(auto& [i, entry] : model.m_entries){
    //     if(!wpi::ends_with(entry->GetName(), "/.type") || !(entry->type == "string") ){
    //         continue;
    //     }
    //     wpi::log::DataLogRecord record;
    //     record = entry->GetNextRecord(0);
    //     if(record.GetEntry() == -1){
    //         continue;
    //     }

    // }
}

