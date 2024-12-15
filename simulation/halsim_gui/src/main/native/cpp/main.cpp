// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>
#include <string_view>

#include <glass/Context.h>
#include <glass/Model.h>
#include <glass/Storage.h>
#include <glass/Window.h>
#include <glass/hardware/AnalogInput.h>
#include <glass/hardware/DIO.h>
#include <glass/hardware/Encoder.h>
#include <glass/hardware/LEDDisplay.h>
#include <glass/hardware/PowerDistribution.h>
#include <glass/hardware/Pneumatic.h>
#include <glass/hardware/PWM.h>
#include <glass/hardware/Relay.h>
#include <glass/hardware/RoboRio.h>
#include <glass/networktables/NetworkTables.h>
#include <glass/other/DeviceTree.h>
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

static glass::NetworkTablesModel* gNetworkTablesModel;
static glass::AnalogInputsModel* gAnalogInputsModel;
static glass::DIOsModel* gDIOsModel;
static glass::EncodersModel* gEncodersModel;
static glass::LEDDisplaysModel* gLEDsModel;
static glass::PowerDistributionsModel* gPowerDistributionsModel;
static glass::PWMsModel* gPWMsModel;
static glass::RelaysModel* gRelaysModel;
static glass::RoboRioModel* gRoboRioModel;
static glass::PneumaticControlsModel* gPCMsModel;
static glass::PneumaticControlsModel* gPHsModel;
static SimDevicesModel* gSimDevicesModel;
static glass::DeviceTreeModel* gDeviceTreeModel;

static glass::Window* gNetworkTablesWindow;
static glass::Window* gNetworkTablesInfoWindow;
static glass::Window* gAnalogInputsWindow;
static glass::Window* gDIOsWindow;
static glass::Window* gEncodersWindow;
static glass::Window* gLEDsWindow;
static glass::Window* gPowerDistributionsWindow;
static glass::Window* gPWMsWindow;
static glass::Window* gRelaysWindow;
static glass::Window* gRoboRioWindow;
static glass::Window* gSolenoidsWindow;
static glass::Window* gTimingWindow;
static glass::Window* gDeviceTreeWindow;

glass::DataSource* halsimgui::GetDeviceValueSource(HAL_SimValueHandle handle) {
  return gSimDevicesModel->GetSource(handle);
}

glass::DeviceTreeModel& halsimgui::GetDeviceTree() {
  return *gDeviceTreeModel;
}

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
  InitializeDS();
  gPlotProvider = std::make_unique<glass::PlotProvider>(
      glass::GetStorageRoot().GetChild("Plot"));
  gPlotProvider->GlobalInit();

  glass::PushStorageStack("NetworkTables");

  gNetworkTablesModel = glass::CreateModel<glass::NetworkTablesModel>();

  gNetworkTablesWindow = glass::imm::CreateWindow("NetworkTables View");
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);

  // NetworkTables info window
  gNetworkTablesInfoWindow = glass::imm::CreateWindow(
      "NetworkTables Info", false, glass::Window::kHide);
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);

  glass::PopStorageStack();

  glass::PushStorageStack("HAL");

  // Models

  // These need to initialize first
  gEncodersModel = CreateEncodersModel();
  gDeviceTreeModel = glass::CreateModel<glass::DeviceTreeModel>();
  gSimDevicesModel = glass::CreateModel<SimDevicesModel>();
  InitializeDeviceTree(*gDeviceTreeModel, gSimDevicesModel);

  InitializeAccelerometers(*gDeviceTreeModel);
  gLEDsModel = CreateAddressableLEDsModel();
  InitializeAnalogGyros(*gDeviceTreeModel);
  gAnalogInputsModel = CreateAnalogInputsModel();
  InitializeAnalogOutputs(*gDeviceTreeModel);
  gDIOsModel = CreateDIOsModel(*gEncodersModel);
  gPCMsModel = CreatePCMsModel();
  InitializePCMs(*gDeviceTreeModel, gPCMsModel);
  gPowerDistributionsModel = CreatePowerDistributionsModel();
  gPWMsModel = CreatePWMsModel();
  gRelaysModel = CreateRelaysModel();
  gPHsModel = CreatePHsModel();
  InitializePHs(*gDeviceTreeModel, gPHsModel);
  gRoboRioModel = CreateRoboRioModel();

  // Windows
  gLEDsWindow =
      glass::imm::CreateWindow("Addressable LEDs", false, glass::Window::kHide);
  gLEDsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gLEDsWindow->SetDefaultPos(290, 100);

  gAnalogInputsWindow =
      glass::imm::CreateWindow("Analog Inputs", false, glass::Window::kHide);
  gAnalogInputsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gAnalogInputsWindow->SetDefaultPos(640, 20);

  gDIOsWindow = glass::imm::CreateWindow("DIO", false, glass::Window::kHide);
  gDIOsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gDIOsWindow->SetDefaultPos(470, 20);

  gEncodersWindow =
      glass::imm::CreateWindow("Encoders", false, glass::Window::kHide);
  gEncodersWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gEncodersWindow->SetDefaultPos(5, 250);

  gPowerDistributionsWindow = glass::imm::CreateWindow(
      "Power Distributions", false, glass::Window::kHide);
  gPowerDistributionsWindow->SetDefaultPos(245, 155);

  gPWMsWindow =
      glass::imm::CreateWindow("PWM Outputs", false, glass::Window::kHide);
  gPWMsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gPWMsWindow->SetDefaultPos(910, 20);

  gRelaysWindow =
      glass::imm::CreateWindow("Relays", false, glass::Window::kHide);
  gRelaysWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gRelaysWindow->SetDefaultPos(180, 20);

  gRoboRioWindow =
      glass::imm::CreateWindow("RoboRIO", false, glass::Window::kHide);
  gRoboRioWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gRoboRioWindow->SetDefaultPos(5, 125);

  gSolenoidsWindow =
      glass::imm::CreateWindow("Solenoids", false, glass::Window::kHide);
  gSolenoidsWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gSolenoidsWindow->SetDefaultPos(290, 20);

  gTimingWindow = glass::imm::CreateWindow("Timing");
  gTimingWindow->DisableRenamePopup();
  gTimingWindow->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
  gTimingWindow->SetDefaultPos(5, 150);

  gDeviceTreeWindow = glass::imm::CreateWindow("Other Devices");
  gDeviceTreeWindow->SetDefaultPos(1025, 20);
  gDeviceTreeWindow->SetDefaultSize(250, 695);
  gDeviceTreeWindow->DisableRenamePopup();

  glass::PopStorageStack();

  HALSimGui::mainMenu.AddMainMenu([] {
    if (ImGui::BeginMenu("Hardware")) {
      DisplayDisableMenuItem();
      ImGui::Separator();
      gLEDsWindow->DisplayMenuItem(nullptr, gLEDsModel->Exists());
      gAnalogInputsWindow->DisplayMenuItem(nullptr,
                                           gAnalogInputsModel->Exists());
      gDIOsWindow->DisplayMenuItem(nullptr, gDIOsModel->Exists());
      gEncodersWindow->DisplayMenuItem(nullptr, gEncodersModel->Exists());
      gPowerDistributionsWindow->DisplayMenuItem(
          nullptr, gPowerDistributionsModel->Exists());
      gPWMsWindow->DisplayMenuItem(nullptr, gPWMsModel->Exists());
      gRelaysWindow->DisplayMenuItem(nullptr, gRelaysModel->Exists());
      gRoboRioWindow->DisplayMenuItem(nullptr, gRoboRioModel->Exists());
      gSolenoidsWindow->DisplayMenuItem(
          nullptr, gPCMsModel->AnySolenoids() || gPHsModel->AnySolenoids());
      ImGui::Separator();
      gTimingWindow->DisplayMenuItem();
      gDeviceTreeWindow->DisplayMenuItem();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("NetworkTables")) {
      gNetworkTablesWindow->DisplayMenuItem();
      gNetworkTablesInfoWindow->DisplayMenuItem();
      ImGui::Separator();
      HALSimGui::ntProvider->DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("DS")) {
      DisplayDSMenu();
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

  wpi::gui::AddEarlyExecute([] { glass::UpdateModels(); });

  wpi::gui::AddLateExecute([] {
    if (glass::imm::BeginWindow(gNetworkTablesWindow)) {
      auto& settings =
          glass::GetStorage().GetOrNewData<glass::NetworkTablesFlagsSettings>();
      if (glass::imm::BeginWindowSettingsPopup()) {
        settings.DisplayMenu();
        glass::DisplayNetworkTablesAddMenu(gNetworkTablesModel, {},
                                           settings.GetFlags());
        ImGui::EndPopup();
      }
      DisplayNetworkTables(gNetworkTablesModel, settings.GetFlags());
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gNetworkTablesInfoWindow)) {
      glass::DisplayNetworkTablesInfo(gNetworkTablesModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gLEDsWindow)) {
      glass::DisplayLEDDisplays(gLEDsModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gAnalogInputsWindow)) {
      glass::DisplayAnalogInputs(gAnalogInputsModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gDIOsWindow)) {
      glass::DisplayDIOs(gDIOsModel, AreOutputsEnabled());
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gEncodersWindow)) {
      glass::DisplayEncoders(gEncodersModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gPowerDistributionsWindow)) {
      DisplayPowerDistributions(gPowerDistributionsModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gPWMsWindow)) {
      glass::DisplayPWMs(gPWMsModel, AreOutputsEnabled());
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gRelaysWindow)) {
      glass::DisplayRelays(gRelaysModel, AreOutputsEnabled());
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gRoboRioWindow)) {
      DisplayRoboRio(gRoboRioModel);
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gSolenoidsWindow)) {
      glass::DisplayPneumaticControlsSolenoids(gPCMsModel, AreOutputsEnabled());
      glass::DisplayPneumaticControlsSolenoids(gPHsModel, AreOutputsEnabled());
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gTimingWindow)) {
      DisplayTiming();
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gDeviceTreeWindow)) {
      DisplayDeviceTree(gDeviceTreeModel);
    }
    glass::imm::EndWindow();

    DisplayDS();
  });

  if (!gui::Initialize("Robot Simulation", 1280, 720,
                       ImGuiConfigFlags_DockingEnable)) {
    return 0;
  }
  HAL_RegisterExtensionListener(nullptr,
                                [](void*, const char* name, void* data) {
                                  if (std::string_view{name} == "ds_socket") {
                                    SetDSSocketExtension(data);
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
