// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Window.h"

#include <string>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "glass/Context.h"
#include "glass/Storage.h"
#include "glass/support/ExtraGuiWidgets.h"

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

  std::string* name = &m_name;
  if (m_name.empty()) {
    name = &m_defaultName;
  }
  std::string label = fmt::format("{}###{}", *name, m_id);

  // Accounts for size of title, collapse button, and close button
  float minWidth =
      ImGui::CalcTextSize(name->c_str()).x + ImGui::GetFontSize() * 2 +
      ImGui::GetStyle().ItemInnerSpacing.x * 3 +
      ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().WindowBorderSize;
  // Accounts for size of hamburger button
  if (m_renamePopupEnabled || m_view->HasSettings()) {
    minWidth += ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.x;
  }
  ImGui::SetNextWindowSizeConstraints({minWidth, 0}, ImVec2{FLT_MAX, FLT_MAX});

  if (Begin(label.c_str(), &m_visible, m_flags)) {
    if (m_renamePopupEnabled || m_view->HasSettings()) {
      bool isClicked = (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
                        ImGui::IsItemHovered());
      ImGuiWindow* window = ImGui::GetCurrentWindow();

      bool settingsButtonClicked = false;
      // Not docked, and window has just enough for the circles not to be
      // touching
      if (!ImGui::IsWindowDocked() &&
          ImGui::GetWindowWidth() > (ImGui::GetFontSize() + 2) * 3 +
                                        ImGui::GetStyle().FramePadding.x * 2) {
        const ImGuiItemFlags itemFlagsRestore =
            ImGui::GetCurrentContext()->CurrentItemFlags;

        ImGui::GetCurrentContext()->CurrentItemFlags |=
            ImGuiItemFlags_NoNavDefaultFocus;
        window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;

        // Allow to draw outside of normal window
        ImGui::PushClipRect(window->OuterRectClipped.Min,
                            window->OuterRectClipped.Max, false);

        const ImRect titleBarRect = ImGui::GetCurrentWindow()->TitleBarRect();
        const ImVec2 position = {titleBarRect.Max.x -
                                     (ImGui::GetStyle().FramePadding.x * 3) -
                                     (ImGui::GetFontSize() * 2),
                                 titleBarRect.Min.y};
        settingsButtonClicked =
            HamburgerButton(ImGui::GetID("#SETTINGS"), position);

        ImGui::PopClipRect();

        ImGui::GetCurrentContext()->CurrentItemFlags = itemFlagsRestore;
      }
      if (settingsButtonClicked || isClicked) {
        ImGui::OpenPopup(window->ID);
      }

      if (ImGui::BeginPopupEx(window->ID,
                              ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoSavedSettings)) {
        if (m_renamePopupEnabled) {
          ItemEditName(&m_name);
        }
        m_view->Settings();

        ImGui::EndPopup();
      }
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
