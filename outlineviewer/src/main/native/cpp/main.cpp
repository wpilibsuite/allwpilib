// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <glass/Context.h>
#include <glass/MainMenuBar.h>
#include <glass/Model.h>
#include <glass/Storage.h>
#include <glass/networktables/NetworkTables.h>
#include <glass/networktables/NetworkTablesSettings.h>
#include <glass/other/Log.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpigui.h>
#include <wpigui_openurl.h>

namespace gui = wpi::gui;

const char* GetWPILibVersion();

namespace ov {
std::string_view GetResource_ov_16_png();
std::string_view GetResource_ov_32_png();
std::string_view GetResource_ov_48_png();
std::string_view GetResource_ov_64_png();
std::string_view GetResource_ov_128_png();
std::string_view GetResource_ov_256_png();
std::string_view GetResource_ov_512_png();
}  // namespace ov

static std::unique_ptr<glass::NetworkTablesModel> gModel;
static std::unique_ptr<glass::NetworkTablesSettings> gSettings;
static glass::LogData gLog;
static glass::NetworkTablesFlagsSettings gFlagsSettings;
static glass::MainMenuBar gMainMenu;
static unsigned int gPrevMode = NT_NET_MODE_NONE;

/**
 * Generates the proper title bar title based on current instance state and
 * event.
 */
static std::string MakeTitle(NT_Inst inst, nt::Event event) {
  auto mode = nt::GetNetworkMode(inst);
  if (mode & NT_NET_MODE_SERVER) {
    auto numClients = nt::GetConnections(inst).size();
    return fmt::format("OutlineViewer - {} Client{} Connected", numClients,
                       (numClients == 1 ? "" : "s"));
  } else if (mode & NT_NET_MODE_CLIENT3 || mode & NT_NET_MODE_CLIENT4) {
    if (event.Is(NT_EVENT_CONNECTED)) {
      return fmt::format("OutlineViewer - Connected ({})",
                         event.GetConnectionInfo()->remote_ip);
    }
  }
  return "OutlineViewer - DISCONNECTED";
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
        // handle NetworkTables log messages
        const char* level = "";
        if (msg->level >= NT_LOG_CRITICAL) {
          level = "CRITICAL: ";
        } else if (msg->level >= NT_LOG_ERROR) {
          level = "ERROR: ";
        } else if (msg->level >= NT_LOG_WARNING) {
          level = "WARNING: ";
        }
        gLog.Append(fmt::format("{}{} ({}:{})\n", level, msg->message,
                                msg->filename, msg->line));
      }
    }

    if (updateTitle) {
      glfwSetWindowTitle(win, MakeTitle(inst, connectionEvent).c_str());
    }
  });

  // NetworkTables table window
  gModel = std::make_unique<glass::NetworkTablesModel>();
  gui::AddEarlyExecute([] { gModel->Update(); });

  // NetworkTables settings window
  gSettings = std::make_unique<glass::NetworkTablesSettings>(
      "outlineviewer",
      glass::GetStorageRoot().GetChild("NetworkTables Settings"));
  gui::AddEarlyExecute([] { gSettings->Update(); });
}

static void DisplayGui() {
  ImGui::GetStyle().WindowRounding = 0;

  // fill entire OS window with this window
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  int width, height;
  glfwGetWindowSize(gui::GetSystemWindow(), &width, &height);
  ImGui::SetNextWindowSize(
      ImVec2(static_cast<float>(width), static_cast<float>(height)));

  ImGui::Begin("Entries", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  gFlagsSettings.Update();

  // can't create popups from within menu, so use flags
  bool settings = false;
  bool log = false;
  bool about = false;

  // main menu
  ImGui::BeginMenuBar();
  gMainMenu.WorkspaceMenu();
  gui::EmitViewMenu();
  if (ImGui::BeginMenu("View")) {
    gFlagsSettings.DisplayMenu();
    glass::DisplayNetworkTablesAddMenu(gModel.get());
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Options")) {
    if (ImGui::MenuItem("Settings")) {
      settings = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Info")) {
    if (ImGui::MenuItem("Log")) {
      log = true;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("About")) {
      about = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Docs")) {
    if (ImGui::MenuItem("Online documentation")) {
      wpi::gui::OpenURL(
          "https://docs.wpilib.org/en/stable/docs/software/wpilib-tools/"
          "outlineviewer/");
    }
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();

  // settings popup
  if (settings) {
    ImGui::OpenPopup("Settings");
  }
  if (ImGui::BeginPopupModal("Settings", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (gSettings->Display()) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // log popup
  if (log) {
    ImGui::OpenPopup("Log");
  }
  if (ImGui::BeginPopupModal("Log", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::BeginChild("Lines", ImVec2(width * 0.75f, height * 0.75f));
    glass::DisplayLog(&gLog, true);
    ImGui::EndChild();
    ImGui::EndPopup();
  }

  // about popup
  if (about) {
    ImGui::OpenPopup("About");
  }
  if (ImGui::BeginPopupModal("About", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("OutlineViewer");
    ImGui::Separator();
    ImGui::Text("v%s", GetWPILibVersion());
    ImGui::Separator();
    ImGui::Text("Save location: %s", glass::GetStorageDir().c_str());
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // display table view
  glass::DisplayNetworkTablesInfo(gModel.get());
  ImGui::Separator();
  glass::DisplayNetworkTables(gModel.get(), gFlagsSettings.GetFlags());

  ImGui::End();
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

  gui::AddIcon(ov::GetResource_ov_16_png());
  gui::AddIcon(ov::GetResource_ov_32_png());
  gui::AddIcon(ov::GetResource_ov_48_png());
  gui::AddIcon(ov::GetResource_ov_64_png());
  gui::AddIcon(ov::GetResource_ov_128_png());
  gui::AddIcon(ov::GetResource_ov_256_png());
  gui::AddIcon(ov::GetResource_ov_512_png());

  glass::SetStorageName("outlineviewer");
  glass::SetStorageDir(saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                       : saveDir);

  gui::AddInit(NtInitialize);

  gui::AddLateExecute(DisplayGui);

  gui::Initialize("OutlineViewer - DISCONNECTED", 600, 400);
  gui::Main();

  gModel.reset();
  gSettings.reset();

  glass::DestroyContext();
  gui::DestroyContext();

  return 0;
}
