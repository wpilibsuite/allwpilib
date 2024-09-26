// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/MainMenuBar.h"

#include <memory>
#include <utility>

#include <imgui.h>
#include <wpi/StringExtras.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/ContextInternal.h"

using namespace glass;

void MainMenuBar::AddMainMenu(std::function<void()> menu) {
  if (menu) {
    m_menus.emplace_back(std::move(menu));
  }
}

void MainMenuBar::AddOptionMenu(std::function<void()> menu) {
  if (menu) {
    m_optionMenus.emplace_back(std::move(menu));
  }
}

void MainMenuBar::Display() {
  ImGui::BeginMainMenuBar();

  WorkspaceMenu();

  if (!m_optionMenus.empty()) {
    if (ImGui::BeginMenu("Options")) {
      for (auto&& menu : m_optionMenus) {
        if (menu) {
          menu();
        }
      }
      ImGui::EndMenu();
    }
  }

  wpi::gui::EmitViewMenu();

  for (auto&& menu : m_menus) {
    if (menu) {
      menu();
    }
  }

#if 0
  char str[64];
  wpi::format_to_n_c_str(str, sizeof(str), "{:.3f} ms/frame ({:.1f} FPS)",
                         1000.0f / ImGui::GetIO().Framerate,
                         ImGui::GetIO().Framerate);

  ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(str).x - 10);
  ImGui::Text("%s", str);
#endif
  ImGui::EndMainMenuBar();
}

void MainMenuBar::WorkspaceMenu() {
  if (ImGui::BeginMenu("Workspace")) {
    if (ImGui::MenuItem("Open...")) {
      m_openFolder =
          std::make_unique<pfd::select_folder>("Choose folder to open");
    }
    if (ImGui::MenuItem("Save As...")) {
      m_saveFolder = std::make_unique<pfd::select_folder>("Choose save folder");
    }
    if (ImGui::MenuItem("Save As Global", nullptr, false,
                        !gContext->isPlatformSaveDir)) {
      SetStorageDir(wpi::gui::GetPlatformSaveFileDir());
      SaveStorage();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Reset")) {
      WorkspaceReset();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Exit")) {
      wpi::gui::Exit();
    }
    ImGui::EndMenu();
  }

  if (m_openFolder && m_openFolder->ready(0)) {
    auto result = m_openFolder->result();
    if (!result.empty()) {
      LoadStorage(result);
    }
    m_openFolder.reset();
  }

  if (m_saveFolder && m_saveFolder->ready(0)) {
    auto result = m_saveFolder->result();
    if (!result.empty()) {
      SetStorageDir(result);
      SaveStorage(result);
    }
    m_saveFolder.reset();
  }
}
