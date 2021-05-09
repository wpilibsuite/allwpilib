// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpi/SmallString.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/Model.h"
#include "glass/networktables/NetworkTables.h"
#include "glass/networktables/NetworkTablesSettings.h"
#include "glass/other/Log.h"

namespace gui = wpi::gui;

const char* GetWPILibVersion();

namespace ov {
wpi::StringRef GetResource_ov_16_png();
wpi::StringRef GetResource_ov_32_png();
wpi::StringRef GetResource_ov_48_png();
wpi::StringRef GetResource_ov_64_png();
wpi::StringRef GetResource_ov_128_png();
wpi::StringRef GetResource_ov_256_png();
wpi::StringRef GetResource_ov_512_png();
}  // namespace ov

static std::unique_ptr<glass::NetworkTablesModel> gModel;
static std::unique_ptr<glass::NetworkTablesSettings> gSettings;
static glass::LogData gLog;
static glass::NetworkTablesFlagsSettings gFlagsSettings;

static void NtInitialize() {
  // update window title when connection status changes
  auto inst = nt::GetDefaultInstance();
  auto poller = nt::CreateConnectionListenerPoller(inst);
  nt::AddPolledConnectionListener(poller, true);
  gui::AddEarlyExecute([inst, poller] {
    auto win = gui::GetSystemWindow();
    if (!win) {
      return;
    }
    bool timedOut;
    for (auto&& event : nt::PollConnectionListener(poller, 0, &timedOut)) {
      if ((nt::GetNetworkMode(inst) & NT_NET_MODE_SERVER) != 0) {
        // for server mode, just print number of clients connected
        wpi::SmallString<64> title;
        glfwSetWindowTitle(win, ("OutlineViewer - " +
                                 wpi::Twine{nt::GetConnections(inst).size()} +
                                 " Clients Connected")
                                    .toNullTerminatedStringRef(title)
                                    .data());
      } else if (event.connected) {
        wpi::SmallString<64> title;
        title = "OutlineViewer - Connected (";
        title += event.conn.remote_ip;
        title += ')';
        glfwSetWindowTitle(win, title.c_str());
      } else {
        glfwSetWindowTitle(win, "OutlineViewer - DISCONNECTED");
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
      gLog.Append(wpi::Twine{level} + msg.message + wpi::Twine{" ("} +
                  msg.filename + wpi::Twine{':'} + wpi::Twine{msg.line} +
                  wpi::Twine{")\n"});
    }
  });

  // NetworkTables table window
  gModel = std::make_unique<glass::NetworkTablesModel>();
  gui::AddEarlyExecute([] { gModel->Update(); });

  // NetworkTables settings window
  gSettings = std::make_unique<glass::NetworkTablesSettings>();
  gui::AddEarlyExecute([] { gSettings->Update(); });
}

static void DisplayGui() {
  ImGui::GetStyle().WindowRounding = 0;

  // fill entire OS window with this window
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  int width, height;
  glfwGetWindowSize(gui::GetSystemWindow(), &width, &height);
  ImGui::SetNextWindowSize(ImVec2(width, height));

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
  gui::EmitViewMenu();
  if (ImGui::BeginMenu("View")) {
    gFlagsSettings.DisplayMenu();
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
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // display table view
  glass::DisplayNetworkTables(gModel.get(), gFlagsSettings.GetFlags());

  ImGui::End();
}

#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
#else
int main() {
#endif
  gui::CreateContext();
  glass::CreateContext();

  gui::AddIcon(ov::GetResource_ov_16_png());
  gui::AddIcon(ov::GetResource_ov_32_png());
  gui::AddIcon(ov::GetResource_ov_48_png());
  gui::AddIcon(ov::GetResource_ov_64_png());
  gui::AddIcon(ov::GetResource_ov_128_png());
  gui::AddIcon(ov::GetResource_ov_256_png());
  gui::AddIcon(ov::GetResource_ov_512_png());

  gui::ConfigurePlatformSaveFile("outlineviewer.ini");
  gui::AddInit(NtInitialize);

  gui::AddLateExecute(DisplayGui);

  gui::Initialize("OutlineViewer - DISCONNECTED", 1024, 768);
  gui::Main();

  gModel.reset();
  gSettings.reset();

  glass::DestroyContext();
  gui::DestroyContext();
}
