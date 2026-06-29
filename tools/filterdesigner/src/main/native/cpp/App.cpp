// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "App.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <memory>
#include <string>
#include <string_view>

#include <imgui.h>

#include "wpi/filterdesigner/ui/GraphEditor.hpp"
#include "wpi/glass/Context.hpp"
#include "wpi/glass/MainMenuBar.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/gui/wpigui.hpp"

namespace gui = wpi::gui;

namespace wpi::filterdesigner {
std::string_view GetResource_filterdesigner_16_png();
std::string_view GetResource_filterdesigner_32_png();
std::string_view GetResource_filterdesigner_48_png();
std::string_view GetResource_filterdesigner_64_png();
std::string_view GetResource_filterdesigner_128_png();
std::string_view GetResource_filterdesigner_256_png();
std::string_view GetResource_filterdesigner_512_png();
}  // namespace wpi::filterdesigner

const char* GetWPILibVersion();

static std::unique_ptr<wpi::filterdesigner::GraphEditor> gGraphEditor;

static void DisplayMainMenu() {
  ImGui::BeginMainMenuBar();

  static wpi::glass::MainMenuBar mainMenu;
  mainMenu.WorkspaceMenu();
  gui::EmitViewMenu();

  bool about = false;
  if (ImGui::BeginMenu("Info")) {
    if (ImGui::MenuItem("About")) {
      about = true;
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  if (about) {
    ImGui::OpenPopup("About");
  }
  if (ImGui::BeginPopupModal("About")) {
    ImGui::Text("Filter Designer");
    ImGui::Separator();
    ImGui::Text("v%s", GetWPILibVersion());
    ImGui::Separator();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

static void DisplayGui() {
  DisplayMainMenu();

  // Single full-viewport host window for the node graph canvas. No title bar,
  // chrome, or movable framing — the tool is now nodes-only and the graph is
  // its entire UI. The canvas itself draws the Save/Open/Add Node toolbar.
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
  constexpr ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  if (ImGui::Begin("Node Graph", nullptr, flags)) {
    gGraphEditor->Display();
  }
  ImGui::End();
}

void Application(std::string_view saveDir) {
  gui::CreateContext();
  wpi::glass::CreateContext();

  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_16_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_32_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_48_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_64_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_128_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_256_png());
  gui::AddIcon(wpi::filterdesigner::GetResource_filterdesigner_512_png());

  wpi::glass::SetStorageName("filterdesigner");
  std::string storageDir{saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                         : saveDir};
  wpi::glass::SetStorageDir(storageDir);

  gGraphEditor = std::make_unique<wpi::filterdesigner::GraphEditor>(storageDir);

  gui::AddLateExecute(DisplayGui);
  gui::Initialize("WPILib Filter Designer", 1280, 720);
  gui::Main();

  gGraphEditor.reset();
  wpi::glass::DestroyContext();
  gui::DestroyContext();
}

#endif  // RUNNING_FILTERDESIGNER_TESTS
