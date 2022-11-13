#include "TimeManager.h"
#include <imgui.h>

using namespace sapphire;

void TimeManager::Display() {
  ImGui::PushID("TimeManager");
  ImGui::Checkbox("Use Offset Limits?", &m_limitOffset);
  ImGui::SameLine();
  ImGui::Checkbox("Manually Set Timestamp Limits?", &m_manualLimits);
  ImGui::Text("Manage Overall Time:");
  ImGui::SliderFloat("timestamp", &m_timestamp, m_timestart, m_timestop);
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
    if(m_timestamp >= m_timestop){
      m_timestamp = m_timestart;
    }
  }
  if(m_manualLimits){
    ImGui::Separator();
    auto maxTimestamp = m_logView.GetMaxTimestamp()/1000000.0;
    ImGui::SliderFloat("Start time", &m_timestart, 0, maxTimestamp);
    ImGui::SliderFloat("Stop time", &m_timestop, 0, maxTimestamp);
    if(ImGui::Button("Set Start Time to Now")){
      m_timestart = m_timestamp;
    }
    ImGui::SameLine();
    if(ImGui::Button("Set Stop Time to Now")){
      m_timestop = m_timestamp;
    }
    m_timestart = std::min(m_timestart, m_timestop);
    m_timestop = std::max(m_timestart, m_timestop);
    ImGui::Separator();
  } else{
    m_timestart = 0;
    m_timestop = m_logView.GetMaxTimestamp()/1000000.0;
  }
  for(auto& log : m_logView.m_logs){
    ImGui::Text("%s's offset", log->m_filename.c_str());
    float max =  m_limitOffset ? std::max(0.0,log->GetMaxTimestamp()/1000000.0 - m_timestamp) : log->GetMaxTimestamp()/1000000.0;
    ImGui::SliderFloat(log->m_filename.c_str(), &log->m_offset ,0, max);
  }
  m_lastTime = ImGui::GetTime();
  ImGui::PopID();
};
