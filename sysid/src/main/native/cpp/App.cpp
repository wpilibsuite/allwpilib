// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#ifndef RUNNING_SYSID_TESTS

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <string_view>

#include <glass/Context.h>
#include <glass/MainMenuBar.h>
#include <glass/Storage.h>
#include <glass/Window.h>
#include <glass/WindowManager.h>
#include <glass/other/Log.h>
#include <imgui.h>
#include <wpi/Logger.h>
#include <wpi/print.h>
#include <wpigui.h>
#include <wpigui_openurl.h>

#include "sysid/view/Analyzer.h"
#include "sysid/view/DataSelector.h"
#include "sysid/view/LogLoader.h"
#include "sysid/view/UILayout.h"

namespace gui = wpi::gui;

static std::unique_ptr<glass::WindowManager> gWindowManager;

glass::Window* gLogLoaderWindow;
glass::Window* gDataSelectorWindow;
glass::Window* gAnalyzerWindow;
glass::Window* gProgramLogWindow;
static glass::MainMenuBar gMainMenu;

glass::LogData gLog;
wpi::Logger gLogger;

const char* GetWPILibVersion();

namespace sysid {
std::string_view GetResource_sysid_16_png();
std::string_view GetResource_sysid_32_png();
std::string_view GetResource_sysid_48_png();
std::string_view GetResource_sysid_64_png();
std::string_view GetResource_sysid_128_png();
std::string_view GetResource_sysid_256_png();
std::string_view GetResource_sysid_512_png();
}  // namespace sysid

void Application(std::string_view saveDir) {
  // Create the wpigui (along with Dear ImGui) and Glass contexts.
  gui::CreateContext();
  glass::CreateContext();

  // Add icons
  gui::AddIcon(sysid::GetResource_sysid_16_png());
  gui::AddIcon(sysid::GetResource_sysid_32_png());
  gui::AddIcon(sysid::GetResource_sysid_48_png());
  gui::AddIcon(sysid::GetResource_sysid_64_png());
  gui::AddIcon(sysid::GetResource_sysid_128_png());
  gui::AddIcon(sysid::GetResource_sysid_256_png());
  gui::AddIcon(sysid::GetResource_sysid_512_png());

  glass::SetStorageName("sysid");
  glass::SetStorageDir(saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                       : saveDir);

  // Add messages from the global sysid logger into the Log window.
  gLogger.SetLogger([](unsigned int level, const char* file, unsigned int line,
                       const char* msg) {
    const char* lvl = "";
    if (level >= wpi::WPI_LOG_CRITICAL) {
      lvl = "CRITICAL: ";
    } else if (level >= wpi::WPI_LOG_ERROR) {
      lvl = "ERROR: ";
    } else if (level >= wpi::WPI_LOG_WARNING) {
      lvl = "WARNING: ";
    } else if (level >= wpi::WPI_LOG_INFO) {
      lvl = "INFO: ";
    } else if (level >= wpi::WPI_LOG_DEBUG) {
      lvl = "DEBUG: ";
    }
    std::string filename = std::filesystem::path{file}.filename().string();
    gLog.Append(fmt::format("{}{} ({}:{})\n", lvl, msg, filename, line));
#ifndef NDEBUG
    wpi::print(stderr, "{}{} ({}:{})\n", lvl, msg, filename, line);
#endif
  });

  gLogger.set_min_level(wpi::WPI_LOG_DEBUG);

  // Initialize window manager and add views.
  auto& storage = glass::GetStorageRoot().GetChild("SysId");
  gWindowManager = std::make_unique<glass::WindowManager>(storage);
  gWindowManager->GlobalInit();

  auto logLoader = std::make_unique<sysid::LogLoader>(storage, gLogger);
  auto dataSelector = std::make_unique<sysid::DataSelector>(storage, gLogger);
  auto analyzer = std::make_unique<sysid::Analyzer>(storage, gLogger);

  logLoader->unload.connect([ds = dataSelector.get()] { ds->Reset(); });
  dataSelector->testdata = [_analyzer = analyzer.get()](auto data) {
    _analyzer->m_data = data;
    _analyzer->AnalyzeData();
  };

  gLogLoaderWindow =
      gWindowManager->AddWindow("Log Loader", std::move(logLoader));

  gDataSelectorWindow =
      gWindowManager->AddWindow("Data Selector", std::move(dataSelector));

  gAnalyzerWindow = gWindowManager->AddWindow("Analyzer", std::move(analyzer));

  gProgramLogWindow = gWindowManager->AddWindow(
      "Program Log", std::make_unique<glass::LogView>(&gLog));

  // Set default positions and sizes for windows.

  // Logger window position/size
  gLogLoaderWindow->SetDefaultPos(sysid::kLogLoaderWindowPos.x,
                                  sysid::kLogLoaderWindowPos.y);
  gLogLoaderWindow->SetDefaultSize(sysid::kLogLoaderWindowSize.x,
                                   sysid::kLogLoaderWindowSize.y);

  // Data selector window position/size
  gDataSelectorWindow->SetDefaultPos(sysid::kDataSelectorWindowPos.x,
                                     sysid::kDataSelectorWindowPos.y);
  gDataSelectorWindow->SetDefaultSize(sysid::kDataSelectorWindowSize.x,
                                      sysid::kDataSelectorWindowSize.y);

  // Analyzer window position/size
  gAnalyzerWindow->SetDefaultPos(sysid::kAnalyzerWindowPos.x,
                                 sysid::kAnalyzerWindowPos.y);
  gAnalyzerWindow->SetDefaultSize(sysid::kAnalyzerWindowSize.x,
                                  sysid::kAnalyzerWindowSize.y);

  // Program log window position/size
  gProgramLogWindow->SetDefaultPos(sysid::kProgramLogWindowPos.x,
                                   sysid::kProgramLogWindowPos.y);
  gProgramLogWindow->SetDefaultSize(sysid::kProgramLogWindowSize.x,
                                    sysid::kProgramLogWindowSize.y);
  gProgramLogWindow->DisableRenamePopup();

  // Configure save file.
  gui::ConfigurePlatformSaveFile("sysid.ini");

  // Add menu bar.
  gui::AddLateExecute([] {
    ImGui::BeginMainMenuBar();
    gMainMenu.WorkspaceMenu();
    gui::EmitViewMenu();

    if (ImGui::BeginMenu("Widgets")) {
      gWindowManager->DisplayMenu();
      ImGui::EndMenu();
    }

    bool about = false;
    if (ImGui::BeginMenu("Info")) {
      if (ImGui::MenuItem("About")) {
        about = true;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Docs")) {
      if (ImGui::MenuItem("Online documentation")) {
        wpi::gui::OpenURL(
            "https://docs.wpilib.org/en/stable/docs/software/pathplanning/"
            "system-identification/");
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    if (about) {
      ImGui::OpenPopup("About");
      about = false;
    }
    if (ImGui::BeginPopupModal("About")) {
      ImGui::Text("SysId: System Identification for Robot Mechanisms");
      ImGui::Separator();
      ImGui::Text("v%s", GetWPILibVersion());
      ImGui::Separator();
      ImGui::Text("Save location: %s", glass::GetStorageDir().c_str());
      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  });

  gui::Initialize("System Identification", sysid::kAppWindowSize.x,
                  sysid::kAppWindowSize.y);
  gui::Main();

  glass::DestroyContext();
  gui::DestroyContext();
}

#endif
