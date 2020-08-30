/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "IniSaverInfo.h"

#include <cstdio>
#include <cstring>

#include <imgui_internal.h>

using namespace halsimgui;

void NameInfo::GetName(char* buf, size_t size, const char* defaultName) {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s###Name%s", m_name, defaultName);
  } else {
    std::snprintf(buf, size, "%s###Name%s", defaultName, defaultName);
  }
}

void NameInfo::GetName(char* buf, size_t size, const char* defaultName,
                       int index) {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s [%d]###Name%d", m_name, index, index);
  } else {
    std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
  }
}

void NameInfo::GetName(char* buf, size_t size, const char* defaultName,
                       int index, int index2) {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s [%d,%d]###Name%d", m_name, index, index2,
                  index);
  } else {
    std::snprintf(buf, size, "%s[%d,%d]###Name%d", defaultName, index, index2,
                  index);
  }
}

bool NameInfo::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name != "name") return false;
  size_t len = (std::min)(value.size(), sizeof(m_name) - 1);
  std::memcpy(m_name, value.data(), len);
  m_name[len] = '\0';
  return true;
}

void NameInfo::WriteIni(ImGuiTextBuffer* out) {
  out->appendf("name=%s\n", m_name);
}

void NameInfo::PushEditNameId(int index) {
  char id[64];
  std::snprintf(id, sizeof(id), "Name%d", index);
  ImGui::PushID(id);
}

void NameInfo::PushEditNameId(const char* name) {
  char id[128];
  std::snprintf(id, sizeof(id), "Name%s", name);
  ImGui::PushID(id);
}

void NameInfo::PopupEditName(int index) {
  char id[64];
  std::snprintf(id, sizeof(id), "Name%d", index);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (ImGui::InputText("##edit", m_name, sizeof(m_name),
                         ImGuiInputTextFlags_EnterReturnsTrue))
      ImGui::CloseCurrentPopup();
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
}

void NameInfo::PopupEditName(const char* name) {
  char id[128];
  std::snprintf(id, sizeof(id), "Name%s", name);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (ImGui::InputText("##edit", m_name, sizeof(m_name),
                         ImGuiInputTextFlags_EnterReturnsTrue))
      ImGui::CloseCurrentPopup();
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
}

bool OpenInfo::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name != "open") return false;
  int num;
  if (value.getAsInteger(10, num)) return true;
  m_open = num;
  return true;
}

void OpenInfo::WriteIni(ImGuiTextBuffer* out) {
  out->appendf("open=%d\n", m_open ? 1 : 0);
}
