#include "TimeManager.h"
#include <imgui.h>

using namespace sapphire;

void TimeManager::Display() {
  ImGui::PushID("TimeManager");
  ImGui::Checkbox("Limit Offset?", &limitOffset);
  ImGui::Text("Manage Overall Time:");
  ImGui::SliderFloat("timestamp", &timestamp ,0, m_logView.GetMaxTimestamp()/1000000.0);
  for(auto& log : m_logView.logs){
    ImGui::Text("%s's offset", log->filename.c_str());
    float max =  limitOffset ? std::max(0.0,log->GetMaxTimestamp()/1000000.0 - timestamp) : log->GetMaxTimestamp()/1000000.0;
    ImGui::SliderFloat(log->filename.c_str(), &log->offset ,0, max);
  }
  ImGui::PopID();
};
