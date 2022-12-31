
#include "TextLog.h"
#include <map>
#include "fmt/format.h"

using namespace sapphire;



void TextLog::DragDropAccept(){
    if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntryData")){
        auto source = *static_cast<EntryData**>(payload->Data);
        EntryDataObject obj{*source};
        data.push_back(obj);
        ReconstructLog();
    }
}

void TextLog::DragDropTarget(){
    if(ImGui::BeginDragDropTarget()){
        DragDropAccept();
        ImGui::EndDragDropTarget();
    }
}

void TextLog::ReconstructLog(){
    std::map<int, std::string> split_log;
    for(auto& entry : data){
        std::map<int, std::string> filtered;
        for(auto it = entry.m_data.m_datapoints.begin(); it != entry.m_data.m_datapoints.end(); it++){
            int timestamp = m_nowRef * 1e6 + *entry.m_data.offset;
            if(it->first >  timestamp){
                break;
            }
            filtered.emplace(it->first, fmt::format("{}: {}", entry.m_data.GetName(), 
                                                    GetFormattedEntryValue(entry.m_data, it->second)));
        }
        split_log.insert(filtered.begin(), filtered.end());
    }
    for(auto& entry : split_log){
        text += fmt::format("[{}m{}s]: {}\n", ((int)(entry.first*1e-6))/60, ((int)(entry.first*1e-6))%60, entry.second);
    }

}

void TextLog::Display() {
    ImGui::PushID(m_name.c_str());
    
    ImGui::Selectable("Drop Data Source Here");
    DragDropTarget();
    ImGui::Text("Beginning of Text Log:");
    ImGui::TextUnformatted(text.c_str());
    
    ImGui::PopID();
}