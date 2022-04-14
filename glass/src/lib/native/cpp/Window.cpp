// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Window.h"

#include <imgui_internal.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/Storage.h"

using namespace glass;

Window::Window(Storage& storage, std::string_view id,
               Visibility defaultVisibility)
    : m_id{id},
      m_name{storage.GetString("name")},
      m_defaultName{id},
      m_visible{storage.GetBool("visible", defaultVisibility != kHide)},
      m_enabled{storage.GetBool("enabled", defaultVisibility != kDisabled)},
      m_defaultVisible{storage.GetValue("visible").boolDefault},
      m_defaultEnabled{storage.GetValue("enabled").boolDefault} {}

void Window::SetVisibility(Visibility visibility) {
  m_visible = visibility != kHide;
  m_enabled = visibility != kDisabled;
}

void Window::SetDefaultVisibility(Visibility visibility) {
  m_defaultVisible = visibility != kHide;
  m_defaultEnabled = visibility != kDisabled;
}

void Window::Display() {
  if (!m_view) {
    return;
  }
  if (!m_visible || !m_enabled) {
    PushID(m_id);
    m_view->Hidden();
    PopID();
    return;
  }

  if (m_posCond != 0) {
    ImGui::SetNextWindowPos(m_pos, m_posCond);
  }
  if (m_sizeCond != 0) {
    ImGui::SetNextWindowSize(m_size, m_sizeCond);
  }
  if (m_setPadding) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_padding);
  }

  char label[128];
  std::snprintf(label, sizeof(label), "%s###%s",
                m_name.empty() ? m_defaultName.c_str() : m_name.c_str(),
                m_id.c_str());

  if (Begin(label, &m_visible, m_flags)) {
    if (m_renamePopupEnabled) {
      PopupEditName(nullptr, &m_name);
    }
    m_view->Display();
  } else {
    m_view->Hidden();
  }
  End();
  if (m_setPadding) {
    ImGui::PopStyleVar();
  }
}

bool Window::DisplayMenuItem(const char* label) {
  bool wasVisible = m_visible;
  ImGui::MenuItem(
      label ? label : (m_name.empty() ? m_id.c_str() : m_name.c_str()), nullptr,
      &m_visible, m_enabled);
  return !wasVisible && m_visible;
}

void Window::ScaleDefault(float scale) {
  if ((m_posCond & ImGuiCond_FirstUseEver) != 0) {
    m_pos.x *= scale;
    m_pos.y *= scale;
  }
  if ((m_sizeCond & ImGuiCond_FirstUseEver) != 0) {
    m_size.x *= scale;
    m_size.y *= scale;
  }
}
