/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/Window.h"

#include <imgui_internal.h>
#include <wpi/StringRef.h>

#include "glass/Context.h"

using namespace glass;

void Window::SetVisibility(Visibility visibility) {
  switch (visibility) {
    case kHide:
      m_visible = false;
      m_enabled = true;
      break;
    case kShow:
      m_visible = true;
      m_enabled = true;
      break;
    case kDisabled:
      m_enabled = false;
      break;
  }
}

void Window::Display() {
  if (!m_view) return;
  if (!m_visible || !m_enabled) {
    PushID(m_id);
    m_view->Hidden();
    PopID();
    return;
  }

  if (m_posCond != 0) ImGui::SetNextWindowPos(m_pos, m_posCond);
  if (m_sizeCond != 0) ImGui::SetNextWindowSize(m_size, m_sizeCond);
  if (m_setPadding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_padding);

  char label[128];
  std::snprintf(label, sizeof(label), "%s###%s",
                m_name.empty() ? m_id.c_str() : m_name.c_str(), m_id.c_str());

  if (Begin(label, &m_visible, m_flags)) {
    if (m_renamePopupEnabled) PopupEditName(nullptr, &m_name);
    m_view->Display();
  } else {
    m_view->Hidden();
  }
  End();
  if (m_setPadding) ImGui::PopStyleVar();
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

void Window::IniReadLine(const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();

  if (name == "name") {
    m_name = value;
  } else if (name == "visible") {
    int num;
    if (value.getAsInteger(10, num)) return;
    m_visible = num;
  } else if (name == "enabled") {
    int num;
    if (value.getAsInteger(10, num)) return;
    m_enabled = num;
  }
}

void Window::IniWriteAll(const char* typeName, ImGuiTextBuffer* out_buf) {
  out_buf->appendf("[%s][%s]\nname=%s\nvisible=%d\nenabled=%d\n\n", typeName,
                   m_id.c_str(), m_name.c_str(), m_visible ? 1 : 0,
                   m_enabled ? 1 : 0);
}
