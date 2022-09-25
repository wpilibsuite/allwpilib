#include "EntryManager.h"
#include "fmt/format.h"
#include <imgui.h>
#include <wpi/DataLogReader.h>
#include "DataLog.h"

using namespace sapphire;


EntryView::EntryView(EntryData *data) {
    this->data = data;
}


void EntryView::Display(bool update, float timestamp){
    ImGui::Text("Entry #%d", data->entry);
    ImGui::TextUnformatted(displayString.c_str());
    
    if(update){
        displayString = fmt::format("'{}' = ", data->name,data->type);

        //From printlog example in wpiutil
        displayString += GetFormattedEntryValue(*data, timestamp);
    }
}

