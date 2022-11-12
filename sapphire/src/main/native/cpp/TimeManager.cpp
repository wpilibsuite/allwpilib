#include "TimeManager.h"
#include <imgui.h>

using namespace sapphire;

void TimeManager::Display() {
  ImGui::PushID("TimeManager");
  ImGui::Checkbox("Limit Offset?", &limitOffset);
  ImGui::Text("Manage Overall Time:");
  ImGui::SliderFloat("timestamp", &timestamp ,0, m_logView.GetMaxTimestamp()/1000000.0);
  ImGui::SameLine();
  ImGui::Checkbox("Play", &isPlaying);
  if(isPlaying){
    double deltaT = (ImGui::GetTime()-lastTime) * selected->m_speed;
    timestamp += deltaT;
    if(ImGui::BeginMenu("Speed")){
      for(auto& option : m_options){
        option.selected = &option == selected;
        if(ImGui::MenuItem(option.m_name.c_str(), "", &option.selected)){
          selected = &option;
        }
      }
      ImGui::EndMenu();
    }
  }
  for(auto& log : m_logView.logs){
    ImGui::Text("%s's offset", log->filename.c_str());
    float max =  limitOffset ? std::max(0.0,log->GetMaxTimestamp()/1000000.0 - timestamp) : log->GetMaxTimestamp()/1000000.0;
    ImGui::SliderFloat(log->filename.c_str(), &log->offset ,0, max);
  }
  lastTime = ImGui::GetTime();
  ImGui::PopID();
};
