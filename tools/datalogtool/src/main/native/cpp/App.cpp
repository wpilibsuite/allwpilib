// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "App.hpp"

#include <memory>
#include <string_view>

#include <imgui.h>
#include <imgui_internal.h>
#include <libssh/libssh.h>

#include "Downloader.hpp"
#include "Exporter.hpp"
#include "wpi/glass/Context.hpp"
#include "wpi/glass/MainMenuBar.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/gui/wpigui.hpp"
#include "wpi/gui/wpigui_openurl.hpp"

namespace gui = wpi::gui;

const char* GetWPILibVersion();

namespace dlt {
std::string_view GetResource_dlt_16_png();
std::string_view GetResource_dlt_32_png();
std::string_view GetResource_dlt_48_png();
std::string_view GetResource_dlt_64_png();
std::string_view GetResource_dlt_128_png();
std::string_view GetResource_dlt_256_png();
std::string_view GetResource_dlt_512_png();
}  // namespace dlt

bool gShutdown = false;

static std::unique_ptr<Downloader> gDownloader;
static bool* gDownloadVisible;
static float gDefaultScale = 1.0;

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
    ImGui::SetNextWindowPos(size, cond);
  }
}

static void DisplayDownload() {
  if (!*gDownloadVisible) {
    return;
  }
  SetNextWindowPos(ImVec2{0, 250}, ImGuiCond_FirstUseEver);
  SetNextWindowSize(ImVec2{375, 260}, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Download", gDownloadVisible)) {
    if (!gDownloader) {
      gDownloader = std::make_unique<Downloader>(
          wpi::glass::GetStorageRoot().GetChild("download"));
    }
    gDownloader->Display();
  }
  ImGui::End();
}

static void DisplayMainMenu() {
  ImGui::BeginMainMenuBar();

  static wpi::glass::MainMenuBar mainMenu;
  mainMenu.WorkspaceMenu();
  gui::EmitViewMenu();

  if (ImGui::BeginMenu("Window")) {
    ImGui::MenuItem("Download", nullptr, gDownloadVisible);
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
          "https://docs.wpilib.org/en/stable/docs/software/telemetry/"
          "datalog-download.html");
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  if (about) {
    ImGui::OpenPopup("About");
  }
  if (ImGui::BeginPopupModal("About")) {
    ImGui::Text("Datalog Tool");
    ImGui::Separator();
    ImGui::Text("v%s", GetWPILibVersion());
    ImGui::Separator();
    ImGui::Text("Save location: %s", wpi::glass::GetStorageDir().c_str());
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
  DisplayInputFiles();
  DisplayEntries();
  DisplayOutput(wpi::glass::GetStorageRoot().GetChild("output"));
  DisplayDownload();
}

void Application(std::string_view saveDir) {
  ssh_init();

  gui::CreateContext();
  wpi::glass::CreateContext();

  // Add icons
  gui::AddIcon(dlt::GetResource_dlt_16_png());
  gui::AddIcon(dlt::GetResource_dlt_32_png());
  gui::AddIcon(dlt::GetResource_dlt_48_png());
  gui::AddIcon(dlt::GetResource_dlt_64_png());
  gui::AddIcon(dlt::GetResource_dlt_128_png());
  gui::AddIcon(dlt::GetResource_dlt_256_png());
  gui::AddIcon(dlt::GetResource_dlt_512_png());

  wpi::glass::SetStorageName("datalogtool");
  wpi::glass::SetStorageDir(saveDir.empty() ? gui::GetPlatformSaveFileDir()
                                            : saveDir);

  gui::AddWindowScaler([](float scale) { gDefaultScale = scale; });
  gui::AddLateExecute(DisplayGui);
  gui::Initialize("Datalog Tool", 925, 510);

  gDownloadVisible = &wpi::glass::GetStorageRoot()
                          .GetChild("download")
                          .GetBool("visible", true);

  gui::Main();

  gShutdown = true;
  wpi::glass::DestroyContext();
  gui::DestroyContext();

  gDownloader.reset();
  ssh_finalize();
}
