// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "App.h"

#include <memory>
#include <string_view>
#include <glass/Window.h>
#include <glass/WindowManager.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <glass/Context.h>
#include <glass/MainMenuBar.h>
#include <glass/Storage.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <wpigui.h>

#include "Selector.h"
#include "Plot.h"
#include "LogViewLoader.h"
#include "TimeManager.h"

using namespace sapphire;

namespace gui = wpi::gui;

const char* GetWPILibVersion();

bool gShutdown = false;

static float gDefaultScale = 1.0;

static std::unique_ptr<glass::WindowManager> windowManager;
static std::unique_ptr<PlotProvider> plotProvider;
static std::unique_ptr<LogViewLoader> viewLoader;

static glass::Window* m_logSelectorWindow;
static glass::Window* m_timeManagerWindow;
static glass::Window* m_plotWindow;
static std::unique_ptr<Selector> m_selector;

static glass::Window* m_entryManagerWindow;

void SetNextWindowPos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot) {
  if ((cond & ImGuiCond_FirstUseEver) != 0) {
    ImGui::SetNextWindowPos(pos * gDefaultScale, cond, pivot);
  } else {
    ImGui::SetNextWindowPos(pos, cond, pivot);
  }
}

void SetNextWindowSize(const ImVec2& size, ImGuiCond cond) {
  if ((cond & ImGuiCond_FirstUseEver) != 0) {
    ImGui::SetNextWindowSize(size * gDefaultScale, cond);
  } else {
    ImGui::SetNextWindowSize(size, cond);
  }
}

static void DisplayMenuBar() {
  ImGui::BeginMainMenuBar();

  static glass::MainMenuBar mainMenu;
  mainMenu.WorkspaceMenu();
  gui::EmitViewMenu();

  bool about = false;
  if(ImGui::BeginMenu("Info")) {
    if(ImGui::MenuItem("About")) {
      about = true;
    }
    ImGui::EndMenu();
  }
  
  windowManager->DisplayMenu();
  plotProvider->DisplayMenu();
  viewLoader->DisplayMenu();
  ImGui::EndMainMenuBar();

  if(about) {
    ImGui::OpenPopup("About");
  }
  if(ImGui::BeginPopupModal("About")) {
    ImGui::Text("Sapphire");
    ImGui::Separator();
    ImGui::Text("v%s", GetWPILibVersion());
    ImGui::Separator();
    ImGui::Text("Save location: %s", glass::GetStorageDir().c_str());
    if(ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void Application(std::string_view saveDir) {
  gui::CreateContext();
  glass::CreateContext();
  
  glass::SetStorageName("sapphire");
  glass::SetStorageDir(saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                      : saveDir);
  
  auto& storage = glass::GetStorageRoot().GetChild("Sapphire");
  auto& plotStorage = glass::GetStorageRoot().GetChild("Plots");
  auto& viewStorage = glass::GetStorageRoot().GetChild("ViewStorage");

  
  windowManager = std::make_unique<glass::WindowManager>(storage);

  std::unique_ptr<Selector> selector = std::make_unique<Selector>();
  auto& logs = selector->GetDataLogs();

  auto  logViewer = std::make_unique<DataLogView>(logs);

  auto  timeManager = std::make_unique<TimeManager>(*logViewer);

  auto& timestamp = logViewer->GetTimestamp();

  plotProvider = std::make_unique<PlotProvider>(plotStorage, timestamp);
  viewLoader = std::make_unique<LogViewLoader>(viewStorage, timestamp);
  
  RegisterLogModels(*viewLoader);
  selector->AddLoader(viewLoader.get());

  windowManager->GlobalInit();
  plotProvider->GlobalInit();
  viewLoader->GlobalInit();

  m_logSelectorWindow = windowManager->AddWindow(
    "Log Selector", std::move(selector));

  m_entryManagerWindow = windowManager->AddWindow(
    "Entry Manager", std::move(logViewer));

  m_timeManagerWindow = windowManager->AddWindow(
    "Time Manager", std::move(timeManager));

  gui::AddWindowScaler([](float scale) { gDefaultScale = scale; });
  gui::AddLateExecute(DisplayMenuBar);
  gui::Initialize("Sapphire", 925, 510);

  gui::Main();
  windowManager.reset();
  plotProvider.reset();
  viewLoader.reset();

  gShutdown = true;
  glass::DestroyContext();
  gui::DestroyContext();
}
