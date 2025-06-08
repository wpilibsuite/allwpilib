// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Window.h"

#include <memory>
#include <string>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "glass/Context.h"
#include "glass/Storage.h"
#include "glass/support/ExtraGuiWidgets.h"

using namespace glass;

Window::Window(Storage& storage, Storage& windowStorage, std::string_view id,
               Visibility defaultVisibility)
    : m_storage{storage},
      m_id{id},
      m_name{windowStorage.GetString("name")},
      m_defaultName{id},
      m_visible{windowStorage.GetBool("visible", defaultVisibility != kHide)},
      m_enabled{
          windowStorage.GetBool("enabled", defaultVisibility != kDisabled)},
      m_defaultVisible{windowStorage.GetValue("visible").boolDefault},
      m_defaultEnabled{windowStorage.GetValue("enabled").boolDefault} {}

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

  if (BeginWindow()) {
    if (m_renamePopupEnabled || m_view->HasSettings()) {
      if (imm::BeginWindowSettingsPopup()) {
        if (m_renamePopupEnabled) {
          EditName();
        }
        m_view->Settings();

        ImGui::EndPopup();
      }
    }

    m_view->Display();
  } else {
    m_view->Hidden();
  }
  EndWindow();
}

bool Window::DisplayMenuItem(const char* label, bool enabled) {
  bool wasVisible = m_visible;
  ImGui::MenuItem(
      label ? label : (m_name.empty() ? m_id.c_str() : m_name.c_str()), nullptr,
      &m_visible, m_visible || (enabled && m_enabled));
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

bool Window::BeginWindow() {
  PushStorageStack(m_storage);

  if (!m_visible || !m_enabled) {
    return false;
  }
  m_inWindow = true;

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

  return ImGui::Begin(label.c_str(), &m_visible, m_flags);
}

void Window::EndWindow() {
  PopStorageStack();
  if (!m_inWindow) {
    return;
  }
  m_inWindow = false;
  ImGui::End();
  if (m_setPadding) {
    ImGui::PopStyleVar();
  }
}

bool imm::BeginWindowSettingsPopup() {
  bool isClicked = (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
                    ImGui::IsItemHovered());
  ImGuiWindow* window = ImGui::GetCurrentWindow();

  bool settingsButtonClicked = false;
  // Not docked, and window has just enough for the circles not to be touching
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

  return ImGui::BeginPopupEx(window->ID, ImGuiWindowFlags_AlwaysAutoResize |
                                             ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoSavedSettings);
}

Window* imm::CreateWindow(Storage& root, std::string_view id, bool duplicateOk,
                          Window::Visibility defaultVisibility) {
  Storage& storage = root.GetChild(id);
  Storage& windowStorage = storage.GetChild("window");
  if (auto window = windowStorage.GetData<Window>()) {
    if (!duplicateOk) {
      fmt::print(stderr, "GUI: ignoring duplicate window '{}'\n", id);
      return nullptr;
    }
    return window;
  }
  windowStorage.SetData(
      std::make_shared<Window>(storage, id, defaultVisibility));
  return windowStorage.GetData<Window>();
}
