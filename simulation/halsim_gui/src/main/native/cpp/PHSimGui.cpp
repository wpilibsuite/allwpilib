// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PHSimGui.h"

#include <cstdio>
#include <memory>
#include <vector>

#include <glass/hardware/Pneumatic.h>
#include <glass/other/DeviceTree.h>
#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/REVPHData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(REVPHCompressorOn, "Compressor On");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(REVPHPressureSwitch, "Pressure Switch");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(REVPHCompressorCurrent,
                                    "Compressor Current");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED2(REVPHSolenoidOutput, "Solenoid");

class CompressorSimModel : public glass::CompressorModel {
 public:
  explicit CompressorSimModel(int32_t index)
      : m_index{index},
        m_running{index},
        m_pressureSwitch{index},
        m_current{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetREVPHInitialized(m_index); }

  glass::DataSource* GetRunningData() override { return &m_running; }
  glass::DataSource* GetEnabledData() override { return nullptr; }
  glass::DataSource* GetPressureSwitchData() override {
    return &m_pressureSwitch;
  }
  glass::DataSource* GetCurrentData() override { return &m_current; }

  void SetRunning(bool val) override {
    HALSIM_SetREVPHCompressorOn(m_index, val);
  }
  void SetEnabled(bool val) override {}
  void SetPressureSwitch(bool val) override {
    HALSIM_SetREVPHPressureSwitch(m_index, val);
  }
  void SetCurrent(double val) override {
    HALSIM_SetREVPHCompressorCurrent(m_index, val);
  }

 private:
  int32_t m_index;
  REVPHCompressorOnSource m_running;
  REVPHPressureSwitchSource m_pressureSwitch;
  REVPHCompressorCurrentSource m_current;
};

class SolenoidSimModel : public glass::SolenoidModel {
 public:
  SolenoidSimModel(int32_t index, int32_t channel)
      : m_index{index}, m_channel{channel}, m_output{index, channel} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetREVPHInitialized(m_index); }

  glass::DataSource* GetOutputData() override { return &m_output; }

  void SetOutput(bool val) override {
    HALSIM_SetREVPHSolenoidOutput(m_index, m_channel, val);
  }

 private:
  int32_t m_index;
  int32_t m_channel;
  REVPHSolenoidOutputSource m_output;
};

class PHSimModel : public glass::PneumaticControlModel {
 public:
  explicit PHSimModel(int32_t index)
      : m_index{index},
        m_compressor{index},
        m_solenoids(HAL_GetNumREVPHChannels()) {}

  void Update() override;

  bool Exists() override { return true; }

  CompressorSimModel* GetCompressor() override { return &m_compressor; }

  void ForEachSolenoid(
      wpi::function_ref<void(glass::SolenoidModel& model, int index)> func)
      override;

  std::string_view GetName() override { return "PH"; }

  int GetNumSolenoids() const { return m_solenoidInitCount; }

 private:
  int32_t m_index;
  CompressorSimModel m_compressor;
  std::vector<std::unique_ptr<SolenoidSimModel>> m_solenoids;
  int m_solenoidInitCount = 0;
};

class PHsSimModel : public glass::PneumaticControlsModel {
 public:
  PHsSimModel() : m_models(HAL_GetNumREVPHModules()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPneumaticControl(
      wpi::function_ref<void(glass::PneumaticControlModel& model, int index)>
          func) override;

 private:
  std::vector<std::unique_ptr<PHSimModel>> m_models;
};
}  // namespace

void PHSimModel::Update() {
  int32_t numChannels = m_solenoids.size();
  m_solenoidInitCount = 0;
  for (int32_t i = 0; i < numChannels; ++i) {
    auto& model = m_solenoids[i];
    if (HALSIM_GetREVPHInitialized(m_index)) {
      if (!model) {
        model = std::make_unique<SolenoidSimModel>(m_index, i);
      }
      ++m_solenoidInitCount;
    } else {
      model.reset();
    }
  }
}

void PHSimModel::ForEachSolenoid(
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

void PHsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetREVPHInitialized(i)) {
      if (!model) {
        model = std::make_unique<PHSimModel>(i);
      }
      model->Update();
    } else {
      model.reset();
    }
  }
}

void PHsSimModel::ForEachPneumaticControl(
    wpi::function_ref<void(glass::PneumaticControlModel& model, int index)>
        func) {
  int32_t numREVPHs = m_models.size();
  for (int32_t i = 0; i < numREVPHs; ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

bool PHSimGui::PHsAnyInitialized() {
  static const int32_t num = HAL_GetNumREVPHModules();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetREVPHInitialized(i)) {
      return true;
    }
  }
  return false;
}

bool PHSimGui::PHsAnySolenoids(glass::PneumaticControlsModel* model) {
  bool any = false;
  static_cast<PHsSimModel*>(model)->ForEachPneumaticControl(
      [&](glass::PneumaticControlModel& REVPH, int) {
        if (static_cast<PHSimModel*>(&REVPH)->GetNumSolenoids() > 0) {
          any = true;
        }
      });
  return any;
}

std::unique_ptr<glass::PneumaticControlsModel> PHSimGui::GetPHsModel() {
  return std::make_unique<PHsSimModel>();
}

void PHSimGui::Initialize() {
  HALSimGui::halProvider->RegisterModel(
      "REVPHs", PHSimGui::PHsAnyInitialized,
      [] { return std::make_unique<PHsSimModel>(); });

  SimDeviceGui::GetDeviceTree().Add(
      HALSimGui::halProvider->GetModel("REVPHs"), [](glass::Model* model) {
        glass::DisplayCompressorsDevice(
            static_cast<PHsSimModel*>(model),
            HALSimGui::halProvider->AreOutputsEnabled());
      });
}
