// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>
#include <string_view>

#include <glass/Context.h>
#include <glass/Storage.h>
#include <glass/hardware/Pneumatic.h>
#include <glass/other/Plot.h>
#include <hal/Extensions.h>
#include <hal/Main.h>
#include <imgui.h>
#include <wpigui.h>

#include "AccelerometerSimGui.h"
#include "AddressableLEDGui.h"
#include "AnalogGyroSimGui.h"
#include "AnalogInputSimGui.h"
#include "AnalogOutputSimGui.h"
#include "DIOSimGui.h"
#include "DriverStationGui.h"
#include "EncoderSimGui.h"
#include "HALSimGui.h"
#include "HALSimGuiExt.h"
#include "NetworkTablesSimGui.h"
#include "PCMSimGui.h"
#include "PHSimGui.h"
#include "PWMSimGui.h"
#include "PowerDistributionSimGui.h"
#include "RelaySimGui.h"
#include "RoboRioSimGui.h"
#include "SimDeviceGui.h"
#include "TimingGui.h"

using namespace halsimgui;

namespace gui = wpi::gui;

static std::unique_ptr<glass::PlotProvider> gPlotProvider;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::puts("Simulator GUI Initializing.");

  gui::CreateContext();
  glass::CreateContext();

  glass::SetStorageName("simgui");

  gui::AddInit([] { ImGui::GetIO().ConfigDockingWithShift = true; });

  HAL_RegisterExtension(HALSIMGUI_EXT_ADDGUIINIT,
                        reinterpret_cast<void*>((AddGuiInitFn)&AddGuiInit));
  HAL_RegisterExtension(
      HALSIMGUI_EXT_ADDGUILATEEXECUTE,
      reinterpret_cast<void*>((AddGuiLateExecuteFn)&AddGuiLateExecute));
  HAL_RegisterExtension(
      HALSIMGUI_EXT_ADDGUIEARLYEXECUTE,
      reinterpret_cast<void*>((AddGuiEarlyExecuteFn)&AddGuiEarlyExecute));
  HAL_RegisterExtension(HALSIMGUI_EXT_GUIEXIT,
                        reinterpret_cast<void*>((GuiExitFn)&GuiExit));

  HALSimGui::GlobalInit();
  DriverStationGui::GlobalInit();
  gPlotProvider = std::make_unique<glass::PlotProvider>(
      glass::GetStorageRoot().GetChild("Plot"));
  gPlotProvider->GlobalInit();

  // These need to initialize first
  EncoderSimGui::Initialize();
  SimDeviceGui::Initialize();

  AccelerometerSimGui::Initialize();
  AddressableLEDGui::Initialize();
  AnalogGyroSimGui::Initialize();
  AnalogInputSimGui::Initialize();
  AnalogOutputSimGui::Initialize();
  DIOSimGui::Initialize();
  NetworkTablesSimGui::Initialize();
  PCMSimGui::Initialize();
  PowerDistributionSimGui::Initialize();
  PWMSimGui::Initialize();
  RelaySimGui::Initialize();
  PHSimGui::Initialize();
  RoboRioSimGui::Initialize();
  TimingGui::Initialize();

  HALSimGui::halProvider->RegisterModel(
      "AllPneumaticControls",
      [] {
        return PCMSimGui::PCMsAnyInitialized() || PHSimGui::PHsAnyInitialized();
      },
      [] {
        return std::make_unique<glass::AllPneumaticControlsModel>(
            PCMSimGui::GetPCMsModel(), PHSimGui::GetPHsModel());
      });

  HALSimGui::halProvider->RegisterView(
      "Solenoids", "AllPneumaticControls",
      [](glass::Model* model) {
        auto pneumaticModel =
            static_cast<glass::AllPneumaticControlsModel*>(model);
        return PCMSimGui::PCMsAnySolenoids(pneumaticModel->pcms.get()) ||
               PHSimGui::PHsAnySolenoids(pneumaticModel->phs.get());
      },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(290, 20);
        return glass::MakeFunctionView([=] {
          auto pneumaticModel =
              static_cast<glass::AllPneumaticControlsModel*>(model);
          glass::DisplayPneumaticControlsSolenoids(
              pneumaticModel->pcms.get(),
              HALSimGui::halProvider->AreOutputsEnabled());
          glass::DisplayPneumaticControlsSolenoids(
              pneumaticModel->phs.get(),
              HALSimGui::halProvider->AreOutputsEnabled());
        });
      });

  HALSimGui::mainMenu.AddMainMenu([] {
    if (ImGui::BeginMenu("Hardware")) {
      HALSimGui::halProvider->DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("NetworkTables")) {
      NetworkTablesSimGui::DisplayMenu();
      ImGui::Separator();
      HALSimGui::ntProvider->DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("DS")) {
      DriverStationGui::dsManager->DisplayMenu();
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
    if (HALSimGui::manager->GetNumWindows() > 0 && ImGui::BeginMenu("Window")) {
      HALSimGui::manager->DisplayMenu();
      ImGui::EndMenu();
    }
  });

  if (!gui::Initialize("Robot Simulation", 1280, 720,
                       ImGuiConfigFlags_DockingEnable)) {
    return 0;
  }
  HAL_RegisterExtensionListener(
      nullptr, [](void*, const char* name, void* data) {
        if (std::string_view{name} == "ds_socket") {
          DriverStationGui::SetDSSocketExtension(data);
        }
      });
  HAL_SetMain(
      nullptr,
      [](void*) {
        gui::Main();
        glass::DestroyContext();
        gui::DestroyContext();
      },
      [](void*) { gui::Exit(); });
  std::puts("Simulator GUI Initialized!");

  return 0;
}
}  // extern "C"
