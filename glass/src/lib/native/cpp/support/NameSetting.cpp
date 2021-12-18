// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/NameSetting.h"

#include <fmt/format.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <wpi/StringExtras.h>

using namespace glass;

void NameSetting::GetName(char* buf, size_t size,
                          const char* defaultName) const {
  if (!m_name.empty()) {
    const auto result = fmt::format_to_n(buf, size - 1, "{}", m_name);
    *result.out = '\0';
  } else {
    const auto result = fmt::format_to_n(buf, size - 1, "{}", defaultName);
    *result.out = '\0';
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index) const {
  if (!m_name.empty()) {
    const auto result =
        fmt::format_to_n(buf, size - 1, "{} [{}]", m_name, index);
    *result.out = '\0';
  } else {
    const auto result =
        fmt::format_to_n(buf, size - 1, "{}[{}]", defaultName, index);
    *result.out = '\0';
  }
}

void NameSetting::GetName(char* buf, size_t size, const char* defaultName,
                          int index, int index2) const {
  if (!m_name.empty()) {
    const auto result =
        fmt::format_to_n(buf, size - 1, "{} [{},{}]", m_name, index, index2);
    *result.out = '\0';
  } else {
    const auto result = fmt::format_to_n(buf, size - 1, "{}[{},{}]",
                                         defaultName, index, index2);
    *result.out = '\0';
  }
}

void NameSetting::GetLabel(char* buf, size_t size,
                           const char* defaultName) const {
  if (!m_name.empty()) {
    const auto result =
        fmt::format_to_n(buf, size - 1, "{}###Name{}", m_name, defaultName);
    *result.out = '\0';
  } else {
    const auto result = fmt::format_to_n(buf, size - 1, "{}###Name{}",
                                         defaultName, defaultName);
    *result.out = '\0';
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index) const {
  if (!m_name.empty()) {
    const auto result = fmt::format_to_n(buf, size - 1, "{} [{}]###Name{}",
                                         m_name, index, index);
    *result.out = '\0';
  } else {
    const auto result = fmt::format_to_n(buf, size - 1, "{}[{}]###Name{}",
                                         defaultName, index, index);
    *result.out = '\0';
  }
}

void NameSetting::GetLabel(char* buf, size_t size, const char* defaultName,
                           int index, int index2) const {
  if (!m_name.empty()) {
    const auto result = fmt::format_to_n(buf, size - 1, "{} [{},{}]###Name{}",
                                         m_name, index, index2, index);
    *result.out = '\0';
  } else {
    const auto result = fmt::format_to_n(buf, size - 1, "{}[{},{}]###Name{}",
                                         defaultName, index, index2, index);
    *result.out = '\0';
  }
}

void NameSetting::PushEditNameId(int index) {
  char id[64];
  const auto result = fmt::format_to_n(id, sizeof(id) - 1, "Name{}", index);
  *result.out = '\0';

  ImGui::PushID(id);
}

void NameSetting::PushEditNameId(const char* name) {
  char id[128];
  const auto result = fmt::format_to_n(id, sizeof(id) - 1, "Name{}", name);
  *result.out = '\0';

  ImGui::PushID(id);
}

bool NameSetting::PopupEditName(int index) {
  bool rv = false;

  char id[64];
  const auto result = fmt::format_to_n(id, sizeof(id) - 1, "Name{}", index);
  *result.out = '\0';

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
  const auto result = fmt::format_to_n(id, sizeof(id) - 1, "Name{}", name);
  *result.out = '\0';

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
