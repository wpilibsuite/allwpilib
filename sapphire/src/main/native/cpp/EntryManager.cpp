#include "EntryManager.h"
#include "fmt/format.h"
#include <imgui.h>
#include <wpi/DataLogReader.h>

using namespace sapphire;

static std::vector<EntryView> entries;

std::string GetFormattedEntryValue(const EntryData& data, wpi::log::DataLogRecord record, int timestamp){
    if (data.start.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
            return fmt::format("  {}\n", val);
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
            return fmt::format("  {}\n", val);
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "string" ||
            data.start.type == "json") {
        std::string_view val;
        record.GetString(&val);
        return fmt::format("  '{}'\n", val);
    } else if (data.start.type == "boolean") {
        bool val;
        if (record.GetBoolean(&val)) {
            return fmt::format("  {}\n", val);
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.start.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return "  invalid\n";
        }
    }
    return "";
    
}

void EntryView::Display(){
    ImGui::Text("Entry #%d", data.start.entry);
    ImGui::InputInt("Timestamp", &timestamp);
    if(ImGui::Button("Update")){
        std::string displayString = fmt::format("<name='{}', type='{}'> [{}]\n", data.start.name,
                 data.start.type, data.finishTimestamp);
        auto timePair = data.datapoints.find(timestamp);
        if(timePair == data.datapoints.end()){ return ;}
        auto record = timePair->second;

        // From printlog example in wpiutil
        displayString = displayString + GetFormattedEntryValue(data, record, timestamp);
    }
    ImGui::Text(displayString.c_str());
}


void EntryManager::Display() {
    static char buf[280] = {0};
    ImGui::Text("Manage Entry Time:");
    ImGui::InputText("Entry Name:", buf, 280);

    ImGui::Text(name.c_str());
    for(auto& entry : entries){
        entry.Display();
    }
}

void EntryManager::FromLogData(const LogData& logData){
    entries.clear();
    
    if(!logData.Exists()){
        return;
    }
    for(const auto& entry : logData.m_entry_list){
        entries.push_back(EntryView(*entry));
    }
}
