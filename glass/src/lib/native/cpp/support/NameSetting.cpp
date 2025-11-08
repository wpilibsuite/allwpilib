// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/support/NameSetting.hpp"

#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "wpi/util/StringExtras.hpp"

using namespace glass;

void NameSetting::GetName(char* buf, size_t size,
                          const char* defaultName) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{}", m_name);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}", defaultName);
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{} [{}]", m_name, index);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}[{}]", defaultName, index);
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index, int index2) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{} [{},{}]", m_name, index, index2);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}[{},{}]", defaultName, index, index2);
  }
}

void NameSetting::GetLabel(char* buf, size_t size,
                           const char* defaultName) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{}###Name{}", m_name, defaultName);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}###Name{}", defaultName, defaultName);
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{} [{}]###Name{}", m_name, index, index);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}[{}]###Name{}", defaultName, index,
                           index);
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index, int index2) const {
  if (!m_name.empty()) {
    wpi::format_to_n_c_str(buf, size, "{} [{},{}]###Name{}", m_name, index,
                           index2, index);
  } else {
    wpi::format_to_n_c_str(buf, size, "{}[{},{}]###Name{}", defaultName, index,
                           index2, index);
  }
}

void NameSetting::PushEditNameId(int index) {
  char id[64];
  wpi::format_to_n_c_str(id, sizeof(id), "Name{}", index);

  ImGui::PushID(id);
}

void NameSetting::PushEditNameId(const char* name) {
  char id[128];
  wpi::format_to_n_c_str(id, sizeof(id), "Name{}", name);

  ImGui::PushID(id);
}

bool NameSetting::PopupEditName(int index) {
  bool rv = false;

  char id[64];
  wpi::format_to_n_c_str(id, sizeof(id), "Name{}", index);

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
  wpi::format_to_n_c_str(id, sizeof(id), "Name{}", name);

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
