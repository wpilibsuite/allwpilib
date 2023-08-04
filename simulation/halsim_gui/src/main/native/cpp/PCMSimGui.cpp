// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PCMSimGui.h"

#include <glass/hardware/PCM.h>
#include <glass/other/DeviceTree.h>

#include <cstdio>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/CTREPCMData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(CTREPCMCompressorOn, "Compressor On");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(CTREPCMClosedLoopEnabled, "Closed Loop");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(CTREPCMPressureSwitch, "Pressure Switch");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(CTREPCMCompressorCurrent, "Comp Current");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED2(CTREPCMSolenoidOutput, "Solenoid");

class CompressorSimModel : public glass::CompressorModel {
 public:
  explicit CompressorSimModel(int32_t index)
      : m_index{index},
        m_running{index},
        m_enabled{index},
        m_pressureSwitch{index},
        m_current{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetCTREPCMInitialized(m_index); }

  glass::DataSource* GetRunningData() override { return &m_running; }
  glass::DataSource* GetEnabledData() override { return &m_enabled; }
  glass::DataSource* GetPressureSwitchData() override {
    return &m_pressureSwitch;
  }
  glass::DataSource* GetCurrentData() override { return &m_current; }

  void SetRunning(bool val) override {
    HALSIM_SetCTREPCMCompressorOn(m_index, val);
  }
  void SetEnabled(bool val) override {
    HALSIM_SetCTREPCMClosedLoopEnabled(m_index, val);
  }
  void SetPressureSwitch(bool val) override {
    HALSIM_SetCTREPCMPressureSwitch(m_index, val);
  }
  void SetCurrent(double val) override {
    HALSIM_SetCTREPCMCompressorCurrent(m_index, val);
  }

 private:
  int32_t m_index;
  CTREPCMCompressorOnSource m_running;
  CTREPCMClosedLoopEnabledSource m_enabled;
  CTREPCMPressureSwitchSource m_pressureSwitch;
  CTREPCMCompressorCurrentSource m_current;
};

class SolenoidSimModel : public glass::SolenoidModel {
 public:
  SolenoidSimModel(int32_t index, int32_t channel)
      : m_index{index}, m_channel{channel}, m_output{index, channel} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetCTREPCMInitialized(m_index); }

  glass::DataSource* GetOutputData() override { return &m_output; }

  void SetOutput(bool val) override {
    HALSIM_SetCTREPCMSolenoidOutput(m_index, m_channel, val);
  }

 private:
  int32_t m_index;
  int32_t m_channel;
  CTREPCMSolenoidOutputSource m_output;
};

class PCMSimModel : public glass::PCMModel {
 public:
  explicit PCMSimModel(int32_t index)
      : m_index{index},
        m_compressor{index},
        m_solenoids(HAL_GetNumCTRESolenoidChannels()) {}

  void Update() override;

  bool Exists() override { return true; }

  CompressorSimModel* GetCompressor() override { return &m_compressor; }

  void ForEachSolenoid(
      wpi::function_ref<void(glass::SolenoidModel& model, int index)> func)
      override;

  int GetNumSolenoids() const { return m_solenoidInitCount; }

 private:
  int32_t m_index;
  CompressorSimModel m_compressor;
  std::vector<std::unique_ptr<SolenoidSimModel>> m_solenoids;
  int m_solenoidInitCount = 0;
};

class PCMsSimModel : public glass::PCMsModel {
 public:
  PCMsSimModel() : m_models(HAL_GetNumCTREPCMModules()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPCM(
      wpi::function_ref<void(glass::PCMModel& model, int index)> func) override;

 private:
  std::vector<std::unique_ptr<PCMSimModel>> m_models;
};
}  // namespace

void PCMSimModel::Update() {
  int32_t numChannels = m_solenoids.size();
  m_solenoidInitCount = 0;
  for (int32_t i = 0; i < numChannels; ++i) {
    auto& model = m_solenoids[i];
    if (HALSIM_GetCTREPCMInitialized(m_index)) {
      if (!model) {
        model = std::make_unique<SolenoidSimModel>(m_index, i);
      }
      ++m_solenoidInitCount;
    } else {
      model.reset();
    }
  }
}

void PCMSimModel::ForEachSolenoid(
    wpi::function_ref<void(glass::SolenoidModel& model, int index)> func) {
  if (m_solenoidInitCount == 0) {
    return;
  }
  int32_t numSolenoids = m_solenoids.size();
  for (int32_t i = 0; i < numSolenoids; ++i) {
    if (auto model = m_solenoids[i].get()) {
      func(*model, i);
    }
  }
}

void PCMsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetCTREPCMInitialized(i)) {
      if (!model) {
        model = std::make_unique<PCMSimModel>(i);
      }
      model->Update();
    } else {
      model.reset();
    }
  }
}

void PCMsSimModel::ForEachPCM(
    wpi::function_ref<void(glass::PCMModel& model, int index)> func) {
  int32_t numCTREPCMs = m_models.size();
  for (int32_t i = 0; i < numCTREPCMs; ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool PCMsAnyInitialized() {
  static const int32_t num = HAL_GetNumCTREPCMModules();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetCTREPCMInitialized(i)) {
      return true;
    }
  }
  return false;
}

void PCMSimGui::Initialize() {
  HALSimGui::halProvider->RegisterModel("CTREPCMs", PCMsAnyInitialized, [] {
    return std::make_unique<PCMsSimModel>();
  });
  HALSimGui::halProvider->RegisterView(
      "Solenoids", "CTREPCMs",
      [](glass::Model* model) {
        bool any = false;
        static_cast<PCMsSimModel*>(model)->ForEachPCM(
            [&](glass::PCMModel& CTREPCM, int) {
              if (static_cast<PCMSimModel*>(&CTREPCM)->GetNumSolenoids() > 0) {
                any = true;
              }
            });
        return any;
      },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(290, 20);
        return glass::MakeFunctionView([=] {
          glass::DisplayPCMsSolenoids(
              static_cast<PCMsSimModel*>(model),
              HALSimGui::halProvider->AreOutputsEnabled());
        });
      });

  SimDeviceGui::GetDeviceTree().Add(
      HALSimGui::halProvider->GetModel("CTREPCMs"), [](glass::Model* model) {
        glass::DisplayCompressorsDevice(
            static_cast<PCMsSimModel*>(model),
            HALSimGui::halProvider->AreOutputsEnabled());
      });
}
