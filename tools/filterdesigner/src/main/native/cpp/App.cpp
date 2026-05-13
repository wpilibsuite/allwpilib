// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "App.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>

#include "wpi/filterdesigner/model/ApplyFilter.hpp"
#include "wpi/filterdesigner/ui/CodeGenView.hpp"
#include "wpi/filterdesigner/ui/DataSourceView.hpp"
#include "wpi/filterdesigner/ui/FilterDesignView.hpp"
#include "wpi/filterdesigner/ui/FrequencyPlotView.hpp"
#include "wpi/filterdesigner/ui/GraphEditor.hpp"
#include "wpi/filterdesigner/ui/PoleZeroView.hpp"
#include "wpi/filterdesigner/ui/ResponsePlotView.hpp"
#include "wpi/filterdesigner/ui/TimePlotView.hpp"
#include "wpi/filterdesigner/ui/TimeResponseView.hpp"
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

static std::unique_ptr<wpi::filterdesigner::DataSourceView> gDataSource;
static std::unique_ptr<wpi::filterdesigner::TimePlotView> gTimePlot;
static std::unique_ptr<wpi::filterdesigner::FrequencyPlotView> gFreqPlot;
static std::unique_ptr<wpi::filterdesigner::FilterDesignView> gDesign;
static std::unique_ptr<wpi::filterdesigner::ResponsePlotView> gResponse;
static std::unique_ptr<wpi::filterdesigner::PoleZeroView> gPoleZero;
static std::unique_ptr<wpi::filterdesigner::TimeResponseView> gTimeResp;
static std::unique_ptr<wpi::filterdesigner::CodeGenView> gCodeGen;
static std::unique_ptr<wpi::filterdesigner::GraphEditor> gGraphEditor;

namespace {
struct FilteredCache {
  std::vector<double> values;
  const wpi::filterdesigner::Signal* signal = nullptr;
  uint64_t signalRevision = 0;
  uint64_t filterVersion = 0;

  void Clear() {
    values.clear();
    signal = nullptr;
    signalRevision = 0;
    filterVersion = 0;
  }
};
}  // namespace

static FilteredCache gFilteredCache;

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

  // Layout columns as fractions of the main viewport. Re-snapping every frame
  // (ImGuiCond_Always) keeps panels tiled as the OS window is resized; the
  // tradeoff is panels can't be dragged/resized manually. Docking would let us
  // have both, but glass doesn't build with the docking branch.
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  const ImVec2 work = viewport->WorkSize;
  const ImVec2 workPos = viewport->WorkPos;

  const float leftW = work.x * 0.22f;
  const float rightW = work.x * 0.30f;
  const float midW = work.x - leftW - rightW;
  const float topH = work.y * 0.58f;
  const float botH = work.y - topH;

  ImGui::SetNextWindowPos({workPos.x, workPos.y}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({leftW, topH}, ImGuiCond_Always);
  if (ImGui::Begin("Data Source")) {
    gDataSource->Display();
  }
  ImGui::End();

  // Re-run the filter over the raw signal when either the signal or the
  // filter changes. Keeps per-frame work to the ImPlot draw for long logs.
  // Signals from sliding-window sources (NT4) keep a stable address and a
  // saturated size, so revision is the only reliable change signal.
  const auto* signalForPlot = gDataSource->SelectedSignal();
  const auto& sectionsOpt = gDesign->Result();
  uint64_t filterVersion = gDesign->Version();
  bool cacheStale = signalForPlot != gFilteredCache.signal ||
                    (signalForPlot && signalForPlot->revision !=
                                          gFilteredCache.signalRevision) ||
                    filterVersion != gFilteredCache.filterVersion;
  if (cacheStale) {
    if (signalForPlot && sectionsOpt.has_value() && !sectionsOpt->empty()) {
      gFilteredCache.values =
          wpi::filterdesigner::ApplyFilter(signalForPlot->values, *sectionsOpt);
    } else {
      gFilteredCache.values.clear();
    }
    gFilteredCache.signal = signalForPlot;
    gFilteredCache.signalRevision = signalForPlot ? signalForPlot->revision : 0;
    gFilteredCache.filterVersion = filterVersion;
  }

  ImGui::SetNextWindowPos({workPos.x + leftW, workPos.y}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({midW, topH}, ImGuiCond_Always);
  if (ImGui::Begin("Time Domain")) {
    gTimePlot->Display(signalForPlot, gFilteredCache.values);
  }
  ImGui::End();

  ImGui::SetNextWindowPos({workPos.x + leftW, workPos.y + topH},
                          ImGuiCond_Always);
  ImGui::SetNextWindowSize({midW, botH}, ImGuiCond_Always);
  if (ImGui::Begin("Frequency")) {
    gFreqPlot->Display(signalForPlot, gFilteredCache.values, filterVersion);
  }
  ImGui::End();

  double inferredFs = signalForPlot ? signalForPlot->sampleRate : 0.0;

  ImGui::SetNextWindowPos({workPos.x, workPos.y + topH}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({leftW, botH}, ImGuiCond_Always);
  if (ImGui::Begin("Filter Design")) {
    gDesign->Display(inferredFs);
  }
  ImGui::End();

  ImGui::SetNextWindowPos({workPos.x + leftW + midW, workPos.y},
                          ImGuiCond_Always);
  ImGui::SetNextWindowSize({rightW, topH}, ImGuiCond_Always);
  if (ImGui::Begin("Response")) {
    if (ImGui::BeginTabBar("##responsetabs")) {
      if (ImGui::BeginTabItem("Bode")) {
        gResponse->Display(gDesign->Result(), gDesign->SampleRate());
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Pole-Zero")) {
        gPoleZero->Display(gDesign->Result());
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Time Response")) {
        gTimeResp->Display(gDesign->Result());
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }
  ImGui::End();

  ImGui::SetNextWindowPos({workPos.x + leftW + midW, workPos.y + topH},
                          ImGuiCond_Always);
  ImGui::SetNextWindowSize({rightW, botH}, ImGuiCond_Always);
  if (ImGui::Begin("Code")) {
    gCodeGen->Display(gDesign->Result(), filterVersion, gDesign->Describe());
  }
  ImGui::End();

  // Floats above the tiled layout while the linear-chain panels are still
  // shipping. M8 demolishes the tiled views and the graph takes the whole
  // window. Size + position are FirstUseEver so users can resize/move and
  // the layout persists via ImGui's .ini.
  ImGui::SetNextWindowPos(
      {workPos.x + work.x * 0.1f, workPos.y + work.y * 0.08f},
      ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({work.x * 0.8f, work.y * 0.85f},
                           ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Node Graph")) {
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

  gDataSource = std::make_unique<wpi::filterdesigner::DataSourceView>();
  gTimePlot = std::make_unique<wpi::filterdesigner::TimePlotView>();
  gFreqPlot = std::make_unique<wpi::filterdesigner::FrequencyPlotView>();
  gDesign = std::make_unique<wpi::filterdesigner::FilterDesignView>();
  gResponse = std::make_unique<wpi::filterdesigner::ResponsePlotView>();
  gPoleZero = std::make_unique<wpi::filterdesigner::PoleZeroView>();
  gTimeResp = std::make_unique<wpi::filterdesigner::TimeResponseView>();
  gCodeGen = std::make_unique<wpi::filterdesigner::CodeGenView>();
  gGraphEditor = std::make_unique<wpi::filterdesigner::GraphEditor>(storageDir);

  gui::AddLateExecute(DisplayGui);
  gui::Initialize("WPILib Filter Designer", 1280, 720);
  gui::Main();

  gFilteredCache.Clear();
  gGraphEditor.reset();
  gCodeGen.reset();
  gTimeResp.reset();
  gPoleZero.reset();
  gResponse.reset();
  gDesign.reset();
  gFreqPlot.reset();
  gTimePlot.reset();
  gDataSource.reset();
  wpi::glass::DestroyContext();
  gui::DestroyContext();
}

#endif  // RUNNING_FILTERDESIGNER_TESTS
