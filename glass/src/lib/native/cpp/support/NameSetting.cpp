// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/NameSetting.h"

#include <cstdio>
#include <cstring>

#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <wpi/StringExtras.h>

using namespace glass;

void NameSetting::GetName(char* buf, size_t size,
                          const char* defaultName) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s", m_name.c_str());
  } else {
    std::snprintf(buf, size, "%s", defaultName);
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s [%d]", m_name.c_str(), index);
  } else {
    std::snprintf(buf, size, "%s[%d]", defaultName, index);
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index, int index2) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s [%d,%d]", m_name.c_str(), index, index2);
  } else {
    std::snprintf(buf, size, "%s[%d,%d]", defaultName, index, index2);
  }
}

void NameSetting::GetLabel(char* buf, size_t size,
                           const char* defaultName) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s###Name%s", m_name.c_str(), defaultName);
  } else {
    std::snprintf(buf, size, "%s###Name%s", defaultName, defaultName);
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s [%d]###Name%d", m_name.c_str(), index, index);
  } else {
    std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index, int index2) const {
  if (!m_name.empty()) {
    std::snprintf(buf, size, "%s [%d,%d]###Name%d", m_name.c_str(), index,
                  index2, index);
  } else {
    std::snprintf(buf, size, "%s[%d,%d]###Name%d", defaultName, index, index2,
                  index);
  }
}

void NameSetting::PushEditNameId(int index) {
  char id[64];
  std::snprintf(id, sizeof(id), "Name%d", index);
  ImGui::PushID(id);
}

void NameSetting::PushEditNameId(const char* name) {
  char id[128];
  std::snprintf(id, sizeof(id), "Name%s", name);
  ImGui::PushID(id);
}

bool NameSetting::PopupEditName(int index) {
  bool rv = false;
  char id[64];
  std::snprintf(id, sizeof(id), "Name%d", index);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (InputTextName("##edit")) {
      rv = true;
    }
    if (ImGui::Button("Close") || ImGui::IsKeyPressedMap(ImGuiKey_Enter) ||
        ImGui::IsKeyPressedMap(ImGuiKey_KeyPadEnter)) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  return rv;
}

bool NameSetting::PopupEditName(const char* name) {
  bool rv = false;
  char id[128];
  std::snprintf(id, sizeof(id), "Name%s", name);
  if (ImGui::BeginPopupContextItem(id)) {
    ImGui::Text("Edit name:");
    if (InputTextName("##edit")) {
      rv = true;
    }
    if (ImGui::Button("Close") || ImGui::IsKeyPressedMap(ImGuiKey_Enter) ||
        ImGui::IsKeyPressedMap(ImGuiKey_KeyPadEnter)) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  return rv;
}

bool NameSetting::InputTextName(const char* label_id,
                                ImGuiInputTextFlags flags) {
  return ImGui::InputText(label_id, &m_name, flags);
}
