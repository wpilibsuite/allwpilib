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

void NameInfo::GetName(char* buf, size_t size, const char* defaultName) const {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s", m_name);
  } else {
    std::snprintf(buf, size, "%s", defaultName);
  }
}

void NameInfo::GetName(char* buf, size_t size, const char* defaultName,
                       int index) const {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s [%d]", m_name, index);
  } else {
    std::snprintf(buf, size, "%s[%d]", defaultName, index);
  }
}

void NameInfo::GetName(char* buf, size_t size, const char* defaultName,
                       int index, int index2) const {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s [%d,%d]", m_name, index, index2);
  } else {
    std::snprintf(buf, size, "%s[%d,%d]", defaultName, index, index2);
  }
}

void NameInfo::GetLabel(char* buf, size_t size, const char* defaultName) const {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s###Name%s", m_name, defaultName);
  } else {
    std::snprintf(buf, size, "%s###Name%s", defaultName, defaultName);
  }
}

void NameInfo::GetLabel(char* buf, size_t size, const char* defaultName,
                        int index) const {
  if (m_name[0] != '\0') {
    std::snprintf(buf, size, "%s [%d]###Name%d", m_name, index, index);
  } else {
    std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
  }
}

void NameInfo::GetLabel(char* buf, size_t size, const char* defaultName,
                        int index, int index2) const {
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

bool NameInfo::PopupEditName(int index) {
  bool rv = false;
  char id[64];
  std::snprintf(id, sizeof(id), "Name%d", index);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (InputTextName("##edit", ImGuiInputTextFlags_EnterReturnsTrue)) {
      ImGui::CloseCurrentPopup();
      rv = true;
    }
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
  return rv;
}

bool NameInfo::PopupEditName(const char* name) {
  bool rv = false;
  char id[128];
  std::snprintf(id, sizeof(id), "Name%s", name);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (InputTextName("##edit", ImGuiInputTextFlags_EnterReturnsTrue)) {
      ImGui::CloseCurrentPopup();
      rv = true;
    }
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
  return rv;
}

bool NameInfo::InputTextName(const char* label_id, ImGuiInputTextFlags flags) {
  return ImGui::InputText(label_id, m_name, sizeof(m_name), flags);
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
