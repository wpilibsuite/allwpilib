#include "TimeManager.h"
#include <imgui.h>

using namespace sapphire;

void TimeManager::Display() {
  ImGui::PushID("TimeManager");
  ImGui::Checkbox("Limit Offset?", &m_limitOffset);
  ImGui::Text("Manage Overall Time:");
  ImGui::SliderFloat("timestamp", &m_timestamp ,0, m_logView.GetMaxTimestamp()/1000000.0);
  ImGui::SameLine();
  ImGui::Checkbox("Play", &m_isPlaying);
  if(m_isPlaying){
    double deltaT = (ImGui::GetTime()-m_lastTime) * m_selected->m_speed;
    m_timestamp += deltaT;
    if(ImGui::BeginMenu("Speed")){
      for(auto& option : m_options){
        option.m_isSelected = &option == m_selected;
        if(ImGui::MenuItem(option.m_name.c_str(), "", &option.m_isSelected)){
          m_selected = &option;
        }
      }
      ImGui::EndMenu();
    }
  }
  for(auto& log : m_logView.m_logs){
    ImGui::Text("%s's offset", log->m_filename.c_str());
    float max =  m_limitOffset ? std::max(0.0,log->GetMaxTimestamp()/1000000.0 - m_timestamp) : log->GetMaxTimestamp()/1000000.0;
    ImGui::SliderFloat(log->m_filename.c_str(), &log->m_offset ,0, max);
  }
  m_lastTime = ImGui::GetTime();
  ImGui::PopID();
};
