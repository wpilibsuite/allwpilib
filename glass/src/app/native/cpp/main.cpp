// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/MainMenuBar.h"
#include "glass/Model.h"
#include "glass/Storage.h"
#include "glass/View.h"
#include "glass/networktables/NetworkTables.h"
#include "glass/networktables/NetworkTablesProvider.h"
#include "glass/networktables/NetworkTablesSettings.h"
#include "glass/other/Log.h"
#include "glass/other/Plot.h"

namespace gui = wpi::gui;

const char* GetWPILibVersion();

namespace glass {
std::string_view GetResource_glass_16_png();
std::string_view GetResource_glass_32_png();
std::string_view GetResource_glass_48_png();
std::string_view GetResource_glass_64_png();
std::string_view GetResource_glass_128_png();
std::string_view GetResource_glass_256_png();
std::string_view GetResource_glass_512_png();
}  // namespace glass

static std::unique_ptr<glass::PlotProvider> gPlotProvider;
static std::unique_ptr<glass::NetworkTablesProvider> gNtProvider;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<glass::NetworkTablesSettings> gNetworkTablesSettings;
static glass::LogData gNetworkTablesLog;
static std::unique_ptr<glass::Window> gNetworkTablesWindow;
static std::unique_ptr<glass::Window> gNetworkTablesInfoWindow;
static std::unique_ptr<glass::Window> gNetworkTablesSettingsWindow;
static std::unique_ptr<glass::Window> gNetworkTablesLogWindow;

static glass::MainMenuBar gMainMenu;
static bool gAbout = false;
static bool gSetEnterKey = false;
static bool gKeyEdit = false;
static int* gEnterKey;
static void (*gPrevKeyCallback)(GLFWwindow*, int, int, int, int);

static void RemapEnterKeyCallback(GLFWwindow* window, int key, int scancode,
                                  int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_RELEASE) {
    if (gKeyEdit) {
      *gEnterKey = key;
      gKeyEdit = false;
    } else if (*gEnterKey == key) {
      key = GLFW_KEY_ENTER;
    }
  }

  if (gPrevKeyCallback) {
    gPrevKeyCallback(window, key, scancode, action, mods);
  }
}

static void NtInitialize() {
  auto inst = nt::GetDefaultInstance();
  auto poller = nt::CreateListenerPoller(inst);
  nt::AddPolledListener(
      poller, inst,
      NT_EVENT_CONNECTION | NT_EVENT_IMMEDIATE | NT_EVENT_LOGMESSAGE);
  gui::AddEarlyExecute([poller] {
    auto win = gui::GetSystemWindow();
    if (!win) {
      return;
    }
    for (auto&& event : nt::ReadListenerQueue(poller)) {
      if (auto connInfo = event.GetConnectionInfo()) {
        // update window title when connection status changes
        if ((event.flags & NT_EVENT_CONNECTED) != 0) {
          glfwSetWindowTitle(
              win, fmt::format("Glass - Connected ({})", connInfo->remote_ip)
                       .c_str());
        } else {
          glfwSetWindowTitle(win, "Glass - DISCONNECTED");
        }
      } else if (auto msg = event.GetLogMessage()) {
        const char* level = "";
        if (msg->level >= NT_LOG_CRITICAL) {
          level = "CRITICAL: ";
        } else if (msg->level >= NT_LOG_ERROR) {
          level = "ERROR: ";
        } else if (msg->level >= NT_LOG_WARNING) {
          level = "WARNING: ";
        }
        gNetworkTablesLog.Append(fmt::format(
            "{}{} ({}:{})\n", level, msg->message, msg->filename, msg->line));
      }
    }
  });

  gNetworkTablesLogWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables Log"),
      "NetworkTables Log", glass::Window::kHide);
  gNetworkTablesLogWindow->SetView(
      std::make_unique<glass::LogView>(&gNetworkTablesLog));
  gNetworkTablesLogWindow->SetDefaultPos(250, 615);
  gNetworkTablesLogWindow->SetDefaultSize(600, 130);
  gNetworkTablesLogWindow->DisableRenamePopup();
  gui::AddLateExecute([] { gNetworkTablesLogWindow->Display(); });

  // NetworkTables table window
  gNetworkTablesModel = std::make_unique<glass::NetworkTablesModel>();
  gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });

  gNetworkTablesWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables View"), "NetworkTables");
  gNetworkTablesWindow->SetView(
      std::make_unique<glass::NetworkTablesView>(gNetworkTablesModel.get()));
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);
  gNetworkTablesWindow->DisableRenamePopup();
  gui::AddLateExecute([] { gNetworkTablesWindow->Display(); });

  // NetworkTables info window
  gNetworkTablesInfoWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables Info"),
      "NetworkTables Info");
  gNetworkTablesInfoWindow->SetView(glass::MakeFunctionView(
      [&] { glass::DisplayNetworkTablesInfo(gNetworkTablesModel.get()); }));
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);
  gNetworkTablesInfoWindow->SetDefaultVisibility(glass::Window::kHide);
  gNetworkTablesInfoWindow->DisableRenamePopup();
  gui::AddLateExecute([] { gNetworkTablesInfoWindow->Display(); });

  // NetworkTables settings window
  gNetworkTablesSettings = std::make_unique<glass::NetworkTablesSettings>(
      "glass", glass::GetStorageRoot().GetChild("NetworkTables Settings"));
  gui::AddEarlyExecute([] { gNetworkTablesSettings->Update(); });

  gNetworkTablesSettingsWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables Settings"),
      "NetworkTables Settings");
  gNetworkTablesSettingsWindow->SetView(
      glass::MakeFunctionView([] { gNetworkTablesSettings->Display(); }));
  gNetworkTablesSettingsWindow->SetDefaultPos(30, 30);
  gNetworkTablesSettingsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gNetworkTablesSettingsWindow->DisableRenamePopup();
  gui::AddLateExecute([] { gNetworkTablesSettingsWindow->Display(); });

  gui::AddWindowScaler([](float scale) {
    // scale default window positions
    gNetworkTablesLogWindow->ScaleDefault(scale);
    gNetworkTablesWindow->ScaleDefault(scale);
    gNetworkTablesSettingsWindow->ScaleDefault(scale);
  });
}

#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
  int argc = __argc;
  char** argv = __argv;
#else
int main(int argc, char** argv) {
#endif
  std::string_view saveDir;
  if (argc == 2) {
    saveDir = argv[1];
  }

  gui::CreateContext();
  glass::CreateContext();

  gui::AddIcon(glass::GetResource_glass_16_png());
  gui::AddIcon(glass::GetResource_glass_32_png());
  gui::AddIcon(glass::GetResource_glass_48_png());
  gui::AddIcon(glass::GetResource_glass_64_png());
  gui::AddIcon(glass::GetResource_glass_128_png());
  gui::AddIcon(glass::GetResource_glass_256_png());
  gui::AddIcon(glass::GetResource_glass_512_png());

  gui::AddEarlyExecute(
      [] { ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); });

  gui::AddInit([] { ImGui::GetIO().ConfigDockingWithShift = true; });

  gPlotProvider = std::make_unique<glass::PlotProvider>(
      glass::GetStorageRoot().GetChild("Plots"));
  gNtProvider = std::make_unique<glass::NetworkTablesProvider>(
      glass::GetStorageRoot().GetChild("NetworkTables"));

  glass::SetStorageName("glass");
  glass::SetStorageDir(saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                       : saveDir);
  gPlotProvider->GlobalInit();
  gui::AddInit([] { glass::ResetTime(); });
  gNtProvider->GlobalInit();
  NtInitialize();

  glass::AddStandardNetworkTablesViews(*gNtProvider);

  gui::AddLateExecute([] { gMainMenu.Display(); });

  gMainMenu.AddMainMenu([] {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Set Enter Key")) {
        gSetEnterKey = true;
      }
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
      if (gNetworkTablesInfoWindow) {
        gNetworkTablesInfoWindow->DisplayMenuItem("NetworkTables Info");
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

    if (ImGui::BeginMenu("Info")) {
      if (ImGui::MenuItem("About")) {
        gAbout = true;
      }
      ImGui::EndMenu();
    }
  });

  gui::AddLateExecute([] {
    if (gAbout) {
      ImGui::OpenPopup("About");
      gAbout = false;
    }
    if (ImGui::BeginPopupModal("About")) {
      ImGui::Text("Glass: A different kind of dashboard");
      ImGui::Separator();
      ImGui::Text("v%s", GetWPILibVersion());
      ImGui::Separator();
      ImGui::Text("Save location: %s", glass::GetStorageDir().c_str());
      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (gSetEnterKey) {
      ImGui::OpenPopup("Set Enter Key");
      gSetEnterKey = false;
    }
    if (ImGui::BeginPopupModal("Set Enter Key")) {
      ImGui::Text("Set the key to use to mean 'Enter'");
      ImGui::Text("This is useful to edit values without the DS disabling");
      ImGui::Separator();

      ImGui::Text("Key:");
      ImGui::SameLine();
      char editLabel[40];
      char nameBuf[32];
      const char* name = glfwGetKeyName(*gEnterKey, 0);
      if (!name) {
        std::snprintf(nameBuf, sizeof(nameBuf), "%d", *gEnterKey);
        name = nameBuf;
      }
      std::snprintf(editLabel, sizeof(editLabel), "%s###edit",
                    gKeyEdit ? "(press key)" : name);
      if (ImGui::SmallButton(editLabel)) {
        gKeyEdit = true;
      }
      ImGui::SameLine();
      if (ImGui::SmallButton("Reset")) {
        *gEnterKey = GLFW_KEY_ENTER;
      }

      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
        gKeyEdit = false;
      }
      ImGui::EndPopup();
    }
  });

  gui::Initialize("Glass - DISCONNECTED", 1024, 768,
                  ImGuiConfigFlags_DockingEnable);
  gEnterKey = &glass::GetStorageRoot().GetInt("enterKey", GLFW_KEY_ENTER);
  if (auto win = gui::GetSystemWindow()) {
    gPrevKeyCallback = glfwSetKeyCallback(win, RemapEnterKeyCallback);
  }
  gui::Main();

  gNetworkTablesSettingsWindow.reset();
  gNetworkTablesLogWindow.reset();
  gNetworkTablesWindow.reset();
  gNetworkTablesModel.reset();
  gNtProvider.reset();
  gPlotProvider.reset();

  glass::DestroyContext();
  gui::DestroyContext();

  return 0;
}
