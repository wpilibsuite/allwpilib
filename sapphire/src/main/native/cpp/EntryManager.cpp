#include "EntryManager.h"
#include "fmt/format.h"
#include <imgui.h>
#include <wpi/DataLogReader.h>
#include "DataLog.h"

using namespace sapphire;

std::string GetFormattedEntryValue(const EntryData& data, wpi::log::DataLogRecord record, int timestamp){
    if (data.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "string" ||
            data.type == "json") {
        std::string_view val;
        record.GetString(&val);
        return fmt::format("  '{}'\n", val);
    } else if (data.type == "boolean") {
        bool val;
        if (record.GetBoolean(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    }
    return "  invalid";
    
}

EntryView::EntryView(EntryData *data, float timestamp) {
    this->data = data;
}


void EntryView::Display(bool update, float timestamp){
    ImGui::Text("Entry #%d", data->entry);
    ImGui::TextUnformatted(displayString.c_str());
    
    if(update){
        displayString = fmt::format("'{}' = ", data->name,data->type);
        int expandedts = static_cast<int>(timestamp*1000000);
        auto record = data->GetRecordAt(expandedts);
        if(record.GetEntry() == -1){ return ;}

        //From printlog example in wpiutil
        displayString += GetFormattedEntryValue(*data, record, timestamp);
    }
}

