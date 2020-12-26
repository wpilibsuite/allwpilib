// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/MainMenuBar.h"

#include <cstdio>

#include <wpigui.h>

using namespace glass;

void MainMenuBar::AddMainMenu(std::function<void()> menu) {
  if (menu)
    m_menus.emplace_back(std::move(menu));
}

void MainMenuBar::AddOptionMenu(std::function<void()> menu) {
  if (menu)
    m_optionMenus.emplace_back(std::move(menu));
}

void MainMenuBar::Display() {
  ImGui::BeginMainMenuBar();

  if (!m_optionMenus.empty()) {
    if (ImGui::BeginMenu("Options")) {
      for (auto&& menu : m_optionMenus) {
        if (menu)
          menu();
      }
      ImGui::EndMenu();
    }
  }

  wpi::gui::EmitViewMenu();

  for (auto&& menu : m_menus) {
    if (menu)
      menu();
  }

#if 0
  char str[64];
  std::snprintf(str, sizeof(str), "%.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
  ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(str).x -
                  10);
  ImGui::Text("%s", str);
#endif
  ImGui::EndMainMenuBar();
}
