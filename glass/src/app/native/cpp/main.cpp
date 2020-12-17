/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
#include "glass/other/Plot.h"

namespace gui = wpi::gui;

static std::unique_ptr<glass::PlotProvider> gPlotProvider;
static std::unique_ptr<glass::NetworkTablesProvider> gNtProvider;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<NetworkTablesSettings> gNetworkTablesSettings;
static glass::Window* gNetworkTablesWindow;
static glass::Window* gNetworkTablesSettingsWindow;

static void NtInitialize() {
  // update window title when connection status changes
  auto inst = nt::GetDefaultInstance();
  auto poller = nt::CreateConnectionListenerPoller(inst);
  nt::AddPolledConnectionListener(poller, true);
  gui::AddEarlyExecute([poller] {
    auto win = gui::GetSystemWindow();
    if (!win) return;
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
      if (ImGui::MenuItem("Reset Time")) glass::ResetTime();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("NetworkTables")) {
      if (gNetworkTablesSettingsWindow)
        gNetworkTablesSettingsWindow->DisplayMenuItem("NetworkTables Settings");
      if (gNetworkTablesWindow)
        gNetworkTablesWindow->DisplayMenuItem("NetworkTables View");
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
    ImGui::EndMainMenuBar();
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
