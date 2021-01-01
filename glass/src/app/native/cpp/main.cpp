// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpi/SmallString.h>
#include <wpigui.h>

#include "NetworkTablesSettings.h"
#include "glass/Context.h"
#include "glass/Model.h"
#include "glass/View.h"
#include "glass/networktables/NetworkTables.h"
#include "glass/networktables/NetworkTablesProvider.h"
#include "glass/other/Log.h"
#include "glass/other/Plot.h"

namespace gui = wpi::gui;

const char* GetWPILibVersion();

namespace glass {
wpi::StringRef GetResource_glass_16_png();
wpi::StringRef GetResource_glass_32_png();
wpi::StringRef GetResource_glass_48_png();
wpi::StringRef GetResource_glass_64_png();
wpi::StringRef GetResource_glass_128_png();
wpi::StringRef GetResource_glass_256_png();
wpi::StringRef GetResource_glass_512_png();
}  // namespace glass

static std::unique_ptr<glass::PlotProvider> gPlotProvider;
static std::unique_ptr<glass::NetworkTablesProvider> gNtProvider;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<NetworkTablesSettings> gNetworkTablesSettings;
static glass::LogData gNetworkTablesLog;
static glass::Window* gNetworkTablesWindow;
static glass::Window* gNetworkTablesSettingsWindow;
static glass::Window* gNetworkTablesLogWindow;

static void NtInitialize() {
  // update window title when connection status changes
  auto inst = nt::GetDefaultInstance();
  auto poller = nt::CreateConnectionListenerPoller(inst);
  nt::AddPolledConnectionListener(poller, true);
  gui::AddEarlyExecute([poller] {
    auto win = gui::GetSystemWindow();
    if (!win) {
      return;
    }
    bool timedOut;
    for (auto&& event : nt::PollConnectionListener(poller, 0, &timedOut)) {
      if (event.connected) {
        wpi::SmallString<64> title;
        title = "Glass - Connected (";
        title += event.conn.remote_ip;
        title += ')';
        glfwSetWindowTitle(win, title.c_str());
      } else {
        glfwSetWindowTitle(win, "Glass - DISCONNECTED");
      }
    }
  });

  // handle NetworkTables log messages
  auto logPoller = nt::CreateLoggerPoller(inst);
  nt::AddPolledLogger(logPoller, NT_LOG_INFO, 100);
  gui::AddEarlyExecute([logPoller] {
    bool timedOut;
    for (auto&& msg : nt::PollLogger(logPoller, 0, &timedOut)) {
      const char* level = "";
      if (msg.level >= NT_LOG_CRITICAL) {
        level = "CRITICAL: ";
      } else if (msg.level >= NT_LOG_ERROR) {
        level = "ERROR: ";
      } else if (msg.level >= NT_LOG_WARNING) {
        level = "WARNING: ";
      }
      gNetworkTablesLog.Append(
          wpi::Twine{level} + msg.message + wpi::Twine{" ("} + msg.filename +
          wpi::Twine{':'} + wpi::Twine{msg.line} + wpi::Twine{")\n"});
    }
  });

  gNetworkTablesLogWindow = gNtProvider->AddWindow(
      "NetworkTables Log",
      std::make_unique<glass::LogView>(&gNetworkTablesLog));
  if (gNetworkTablesLogWindow) {
    gNetworkTablesLogWindow->SetDefaultPos(250, 615);
    gNetworkTablesLogWindow->SetDefaultSize(600, 130);
    gNetworkTablesLogWindow->SetVisible(false);
    gNetworkTablesLogWindow->DisableRenamePopup();
  }

  // NetworkTables table window
  gNetworkTablesModel = std::make_unique<glass::NetworkTablesModel>();
  gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });

  gNetworkTablesWindow = gNtProvider->AddWindow(
      "NetworkTables",
      std::make_unique<glass::NetworkTablesView>(gNetworkTablesModel.get()));
  if (gNetworkTablesWindow) {
    gNetworkTablesWindow->SetDefaultPos(250, 277);
    gNetworkTablesWindow->SetDefaultSize(750, 185);
    gNetworkTablesWindow->DisableRenamePopup();
  }

  // NetworkTables settings window
  gNetworkTablesSettings = std::make_unique<NetworkTablesSettings>();
  gui::AddEarlyExecute([] { gNetworkTablesSettings->Update(); });

  gNetworkTablesSettingsWindow = gNtProvider->AddWindow(
      "NetworkTables Settings", [] { gNetworkTablesSettings->Display(); });
  if (gNetworkTablesSettingsWindow) {
    gNetworkTablesSettingsWindow->SetDefaultPos(30, 30);
    gNetworkTablesSettingsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
    gNetworkTablesSettingsWindow->DisableRenamePopup();
  }
}

#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
#else
int main() {
#endif
  gui::CreateContext();
  glass::CreateContext();

  gui::AddIcon(glass::GetResource_glass_16_png());
  gui::AddIcon(glass::GetResource_glass_32_png());
  gui::AddIcon(glass::GetResource_glass_48_png());
  gui::AddIcon(glass::GetResource_glass_64_png());
  gui::AddIcon(glass::GetResource_glass_128_png());
  gui::AddIcon(glass::GetResource_glass_256_png());
  gui::AddIcon(glass::GetResource_glass_512_png());

  gPlotProvider = std::make_unique<glass::PlotProvider>("Plot");
  gNtProvider = std::make_unique<glass::NetworkTablesProvider>("NTProvider");

  gui::ConfigurePlatformSaveFile("glass.ini");
  gPlotProvider->GlobalInit();
  gui::AddInit([] { glass::ResetTime(); });
  gNtProvider->GlobalInit();
  gui::AddInit(NtInitialize);

  glass::AddStandardNetworkTablesViews(*gNtProvider);

  gui::AddLateExecute([] {
    ImGui::BeginMainMenuBar();
    gui::EmitViewMenu();
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Reset Time")) {
        glass::ResetTime();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("NetworkTables")) {
      if (gNetworkTablesSettingsWindow) {
        gNetworkTablesSettingsWindow->DisplayMenuItem("NetworkTables Settings");
      }
      if (gNetworkTablesWindow) {
        gNetworkTablesWindow->DisplayMenuItem("NetworkTables View");
      }
      if (gNetworkTablesLogWindow) {
        gNetworkTablesLogWindow->DisplayMenuItem("NetworkTables Log");
      }
      ImGui::Separator();
      gNtProvider->DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Plot")) {
      bool paused = gPlotProvider->IsPaused();
      if (ImGui::MenuItem("Pause All Plots", nullptr, &paused)) {
        gPlotProvider->SetPaused(paused);
      }
      ImGui::Separator();
      gPlotProvider->DisplayMenu();
      ImGui::EndMenu();
    }

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
      about = false;
    }
    if (ImGui::BeginPopupModal("About")) {
      ImGui::Text("Glass: A different kind of dashboard");
      ImGui::Separator();
      ImGui::Text("v%s", GetWPILibVersion());
      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  });

  gui::Initialize("Glass - DISCONNECTED", 1024, 768);
  gui::Main();

  gNetworkTablesModel.reset();
  gNetworkTablesSettings.reset();
  gNtProvider.reset();
  gPlotProvider.reset();

  glass::DestroyContext();
  gui::DestroyContext();
}
