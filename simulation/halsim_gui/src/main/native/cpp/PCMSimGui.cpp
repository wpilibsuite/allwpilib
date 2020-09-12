/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PCMSimGui.h"

#include <glass/hardware/PCM.h>
#include <glass/other/DeviceTree.h>

#include <cstdio>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/PCMData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMCompressorOn, "Compressor On");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMClosedLoopEnabled, "Closed Loop");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMPressureSwitch, "Pressure Switch");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PCMCompressorCurrent, "Comp Current");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED2(PCMSolenoidOutput, "Solenoid");

class CompressorSimModel : public glass::CompressorModel {
 public:
  explicit CompressorSimModel(int32_t index)
      : m_index{index},
        m_running{index},
        m_enabled{index},
        m_pressureSwitch{index},
        m_current{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetPCMCompressorInitialized(m_index); }

  glass::DataSource* GetRunningData() override { return &m_running; }
  glass::DataSource* GetEnabledData() override { return &m_enabled; }
  glass::DataSource* GetPressureSwitchData() override {
    return &m_pressureSwitch;
  }
  glass::DataSource* GetCurrentData() override { return &m_current; }

  void SetRunning(bool val) override {
    HALSIM_SetPCMCompressorOn(m_index, val);
  }
  void SetEnabled(bool val) override {
    HALSIM_SetPCMClosedLoopEnabled(m_index, val);
  }
  void SetPressureSwitch(bool val) override {
    HALSIM_SetPCMPressureSwitch(m_index, val);
  }
  void SetCurrent(double val) override {
    HALSIM_SetPCMCompressorCurrent(m_index, val);
  }

 private:
  int32_t m_index;
  PCMCompressorOnSource m_running;
  PCMClosedLoopEnabledSource m_enabled;
  PCMPressureSwitchSource m_pressureSwitch;
  PCMCompressorCurrentSource m_current;
};

class SolenoidSimModel : public glass::SolenoidModel {
 public:
  SolenoidSimModel(int32_t index, int32_t channel)
      : m_index{index}, m_channel{channel}, m_output{index, channel} {}

  void Update() override {}

  bool Exists() override {
    return HALSIM_GetPCMSolenoidInitialized(m_index, m_channel);
  }

  glass::DataSource* GetOutputData() override { return &m_output; }

  void SetOutput(bool val) override {
    HALSIM_SetPCMSolenoidOutput(m_index, m_channel, val);
  }

 private:
  int32_t m_index;
  int32_t m_channel;
  PCMSolenoidOutputSource m_output;
};

class PCMSimModel : public glass::PCMModel {
 public:
  explicit PCMSimModel(int32_t index)
      : m_index{index},
        m_compressor{index},
        m_solenoids(HAL_GetNumSolenoidChannels()) {}

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
  PCMsSimModel() : m_models(HAL_GetNumPCMModules()) {}

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
    if (HALSIM_GetPCMSolenoidInitialized(m_index, i)) {
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
  if (m_solenoidInitCount == 0) return;
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
    if (HALSIM_GetPCMCompressorInitialized(i) ||
        HALSIM_GetPCMAnySolenoidInitialized(i)) {
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
  int32_t numPCMs = m_models.size();
  for (int32_t i = 0; i < numPCMs; ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool PCMsAnyInitialized() {
  static const int32_t num = HAL_GetNumPCMModules();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetPCMCompressorInitialized(i) ||
        HALSIM_GetPCMAnySolenoidInitialized(i))
      return true;
  }
  return false;
}

void PCMSimGui::Initialize() {
  HALSimGui::halProvider.RegisterModel("PCMs", PCMsAnyInitialized, [] {
    return std::make_unique<PCMsSimModel>();
  });
  HALSimGui::halProvider.RegisterView(
      "Solenoids", "PCMs",
      [](glass::Model* model) {
        bool any = false;
        static_cast<PCMsSimModel*>(model)->ForEachPCM(
            [&](glass::PCMModel& pcm, int) {
              if (static_cast<PCMSimModel*>(&pcm)->GetNumSolenoids() > 0)
                any = true;
            });
        return any;
      },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(290, 20);
        return glass::MakeFunctionView([=] {
          glass::DisplayPCMsSolenoids(
              static_cast<PCMsSimModel*>(model),
              HALSimGui::halProvider.AreOutputsEnabled());
        });
      });

  SimDeviceGui::GetDeviceTree().Add(
      HALSimGui::halProvider.GetModel("PCMs"), [](glass::Model* model) {
        glass::DisplayCompressorsDevice(
            static_cast<PCMsSimModel*>(model),
            HALSimGui::halProvider.AreOutputsEnabled());
      });
}
