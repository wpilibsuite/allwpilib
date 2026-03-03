// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <cscore_cpp.h>
#include <ntcore_cpp.h>
#include <wpi/StringExtras.h>
#include <wpigui.h>
#include <wpigui_openurl.h>

#include "glass/Context.h"
#include "glass/MainMenuBar.h"
#include "glass/Storage.h"
#include "glass/View.h"
#include "glass/Window.h"
#include "glass/camera/Camera.h"
#include "glass/camera/NTCameraProvider.h"
#include "glass/camera/UsbCameraList.h"
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
static std::unique_ptr<glass::NTCameraProvider> gNtCameraProvider;
static std::unique_ptr<glass::UsbCameraList> gUsbCameraList;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<glass::NetworkTablesSettings> gNetworkTablesSettings;
static glass::LogData gNetworkTablesLog;
static std::unique_ptr<glass::Window> gNetworkTablesWindow;
static std::unique_ptr<glass::Window> gNetworkTablesInfoWindow;
static std::unique_ptr<glass::Window> gNetworkTablesSettingsWindow;
static std::unique_ptr<glass::Window> gNetworkTablesLogWindow;

static glass::Window* gCameraListWindow;

static glass::MainMenuBar gMainMenu;
static bool gAbout = false;
static bool gSetEnterKey = false;
static bool gKeyEdit = false;
static int* gEnterKey;
static void (*gPrevKeyCallback)(GLFWwindow*, int, int, int, int);
static bool gNetworkTablesDebugLog = false;
static unsigned int gPrevMode = NT_NET_MODE_NONE;

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

/**
 * Generates the proper title bar title based on current instance state and
 * event.
 */
static std::string MakeTitle(NT_Inst inst, nt::Event event) {
  auto mode = nt::GetNetworkMode(inst);
  if (mode & NT_NET_MODE_SERVER) {
    auto numClients = nt::GetConnections(inst).size();
    return fmt::format("Glass - {} Client{} Connected", numClients,
                       (numClients == 1 ? "" : "s"));
  } else if (mode & NT_NET_MODE_CLIENT3 || mode & NT_NET_MODE_CLIENT4) {
    if (event.Is(NT_EVENT_CONNECTED)) {
      return fmt::format("Glass - Connected ({})",
                         event.GetConnectionInfo()->remote_ip);
    }
  }
  return "Glass - DISCONNECTED";
}

static void NtInitialize() {
  auto inst = nt::GetDefaultInstance();
  auto poller = nt::CreateListenerPoller(inst);
  nt::AddPolledListener(poller, inst, NT_EVENT_CONNECTION | NT_EVENT_IMMEDIATE);
  nt::AddPolledLogger(poller, NT_LOG_INFO, 100);
  gui::AddEarlyExecute([inst, poller] {
    auto win = gui::GetSystemWindow();
    if (!win) {
      return;
    }
    bool updateTitle = false;
    nt::Event connectionEvent;
    if (nt::GetNetworkMode(inst) != gPrevMode) {
      gPrevMode = nt::GetNetworkMode(inst);
      updateTitle = true;
    }

    for (auto&& event : nt::ReadListenerQueue(poller)) {
      if (event.Is(NT_EVENT_CONNECTION)) {
        updateTitle = true;
        connectionEvent = event;
      } else if (auto msg = event.GetLogMessage()) {
        const char* level = "";
        if (msg->level >= NT_LOG_CRITICAL) {
          level = "CRITICAL: ";
        } else if (msg->level >= NT_LOG_ERROR) {
          level = "ERROR: ";
        } else if (msg->level >= NT_LOG_WARNING) {
          level = "WARNING: ";
        } else if (msg->level < NT_LOG_INFO && !gNetworkTablesDebugLog) {
          continue;
        }
        gNetworkTablesLog.Append(fmt::format(
            "{}{} ({}:{})\n", level, msg->message, msg->filename, msg->line));
      }
    }

    if (updateTitle) {
      glfwSetWindowTitle(win, MakeTitle(inst, connectionEvent).c_str());
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

static void CsInitialize() {
  cs::SetTelemetryPeriod(1.0);
  cs::SetLogger([](unsigned int level, const char* file, unsigned int line,
                   const char* msg) { fmt::print("CS: {}\n", msg); },
                CS_LOG_INFO);

  gNtCameraProvider = std::make_unique<glass::NTCameraProvider>(
      glass::GetStorageRoot().GetChild("NT Cameras"));
  gNtCameraProvider->GlobalInit();

  gUsbCameraList = std::make_unique<glass::UsbCameraList>();

  gCameraListWindow =
      glass::imm::CreateWindow("Camera List", false, glass::Window::kHide);
  gCameraListWindow->SetDefaultPos(250, 250);
  gCameraListWindow->SetDefaultSize(500, 200);
  gCameraListWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gui::AddWindowScaler(
      [](float scale) { gCameraListWindow->ScaleDefault(scale); });

  gui::AddEarlyExecute([] {
    glass::Storage& cameras = glass::GetStorageRoot().GetChild("cameras");
    for (auto&& storage : cameras.GetChildren()) {
      glass::CameraModel* model = glass::GetCameraModel(cameras, storage.key());
      if (!model) {
        model = glass::GetOrNewCameraModel(cameras, storage.key());
        model->Start();
      }
      model->Update();
    }

    gUsbCameraList->Update();
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
  CsInitialize();

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
      ImGui::MenuItem("NetworkTables Debug Logging", nullptr,
                      &gNetworkTablesDebugLog);
      ImGui::Separator();
      gNtProvider->DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Camera")) {
      gCameraListWindow->DisplayMenuItem();
      if (ImGui::MenuItem("New Camera View")) {
        auto& viewsStorage = glass::GetStorageRoot().GetChild("camera views");
        auto& views = viewsStorage.GetValues();
        // this is an inefficient algorithm, but the number of windows is small
        char id[32];
        size_t numViews = views.size();
        for (size_t i = 0; i <= numViews; ++i) {
          wpi::format_to_n_c_str(id, sizeof(id), "Camera View <{}>",
                                 static_cast<int>(i));
          if (!views.contains(id)) {
            break;
          }
        }
        if (auto win = glass::imm::CreateWindow(viewsStorage, id)) {
          win->SetDefaultSize(700, 400);
        }
      }
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

    if (ImGui::BeginMenu("Docs")) {
      if (ImGui::MenuItem("Online documentation")) {
        wpi::gui::OpenURL(
            "https://docs.wpilib.org/en/stable/docs/software/dashboards/"
            "glass/");
      }
      ImGui::EndMenu();
    }
  });

  gui::AddLateExecute([] {
    glass::Storage& cameras = glass::GetStorageRoot().GetChild("cameras");

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
      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
        wpi::format_to_n_c_str(nameBuf, sizeof(nameBuf), "{}", *gEnterKey);

        name = nameBuf;
      }
      wpi::format_to_n_c_str(editLabel, sizeof(editLabel), "{}###edit",
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

  // cs::Shutdown();
  gUsbCameraList.reset();
  gNtCameraProvider.reset();

  gNetworkTablesModel.reset();
  gNtProvider.reset();
  gPlotProvider.reset();

  glass::DestroyContext();
  gui::DestroyContext();

  return 0;
}
