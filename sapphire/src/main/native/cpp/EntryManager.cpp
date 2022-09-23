#include "EntryManager.h"
#include "fmt/format.h"
#include <imgui.h>
#include <wpi/DataLogReader.h>

using namespace sapphire;

static std::vector<EntryView> entries;
static float maxTimestamp= 100;

std::string GetFormattedEntryValue(const EntryData& data, wpi::log::DataLogRecord record, int timestamp){
    if (data.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
            return fmt::format("  {}\n", val);
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
            return fmt::format("  {}\n", val);
        } else {
            return fmt::format("  invalid\n");
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
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return fmt::format("  invalid\n");
        }
    } else if (data.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        } else {
            return "  invalid\n";
        }
    }
    return "  invalid";
    
}

EntryView::EntryView(EntryData *data, float timestamp) {
    this->data = data;
    displayString = fmt::format("<name='{}', type='{}'> [{}]\n", data->name,
                data->type, data->finishTimestamp / 1000000.0);
    int ts = static_cast<int>(timestamp*1000000);
    auto timePair = data->datapoints.find(timestamp);
    if(timePair == data->datapoints.end()){ return ;}
    auto record = timePair->second;

    //From printlog example in wpiutil
    displayString = displayString + GetFormattedEntryValue(*data, record, timestamp);
}


void EntryView::Display(bool update, float timestamp){
    ImGui::Text("Entry #%d", data->entry);
    ImGui::TextUnformatted(displayString.c_str());
    if(update){
        std::string displayString = fmt::format("<name='{}', type='{}'> [{}]\n", data->name,
                 data->type, timestamp);
        int ts = static_cast<int>(timestamp*1000000);
        auto timePair = data->datapoints.find(timestamp);
        if(timePair == data->datapoints.end()){ return ;}
        auto record = timePair->second;

        //From printlog example in wpiutil
        displayString = displayString + GetFormattedEntryValue(*data, record, timestamp);
    }
}


void EntryManager::Display() {
    ImGui::Text("Manage Entry Time:");
    ImGui::SameLine();
    ImGui::SliderFloat("Timestamp", &timestamp ,0, maxTimestamp);
    bool update = ImGui::Button("Update");
    if(update){

    }
    for(auto& entry : entries){
        entry.Display(update, timestamp);
    }
    ImGui::Text("Entry Information: #Entries: %d, Max Timestamp: %d", entries.size(), maxTimestamp);
}

void EntryManager::FromLogData(LogData& logData){
    entries.clear();

    if(!logData.Exists()){
        return;
    }
    maxTimestamp = logData.GetMaxTimestamp() / 1000000.0;
    for(auto& entry : logData.m_entries){
       EntryView view{&entry.second};
       entries.emplace_back(view);
    }

}
