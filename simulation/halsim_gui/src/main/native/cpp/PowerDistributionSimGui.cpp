// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PowerDistributionSimGui.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <glass/hardware/PowerDistribution.h>
#include <hal/Ports.h>
#include <hal/simulation/PowerDistributionData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PowerDistributionTemperature,
                                    "Power Distribution Temp");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PowerDistributionVoltage,
                                    "Power Distribution Voltage");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED2(PowerDistributionCurrent,
                                     "Power Distribution Current");

class PowerDistributionSimModel : public glass::PowerDistributionModel {
 public:
  explicit PowerDistributionSimModel(int32_t index)
      : m_index{index}, m_temp{index}, m_voltage{index} {
    // TODO make this better
    const int numChannels = HAL_GetNumREVPDHChannels();
    m_currents.reserve(numChannels);
    for (int i = 0; i < numChannels; ++i) {
      m_currents.emplace_back(
          std::make_unique<PowerDistributionCurrentSource>(index, i));
    }
  }

  void Update() override {}

  bool Exists() override {
    return HALSIM_GetPowerDistributionInitialized(m_index);
  }

  int GetNumChannels() const override { return m_currents.size(); }

  glass::DataSource* GetTemperatureData() override { return &m_temp; }
  glass::DataSource* GetVoltageData() override { return &m_voltage; }
  glass::DataSource* GetCurrentData(int channel) override {
    return m_currents[channel].get();
  }

  void SetTemperature(double val) override {
    HALSIM_SetPowerDistributionTemperature(m_index, val);
  }
  void SetVoltage(double val) override {
    HALSIM_SetPowerDistributionVoltage(m_index, val);
  }
  void SetCurrent(int channel, double val) override {
    HALSIM_SetPowerDistributionCurrent(m_index, channel, val);
  }

 private:
  int32_t m_index;
  PowerDistributionTemperatureSource m_temp;
  PowerDistributionVoltageSource m_voltage;
  std::vector<std::unique_ptr<PowerDistributionCurrentSource>> m_currents;
};

class PowerDistributionsSimModel : public glass::PowerDistributionsModel {
 public:
  PowerDistributionsSimModel() : m_models(HAL_GetNumREVPDHModules()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPowerDistribution(
      wpi::function_ref<void(glass::PowerDistributionModel& model, int index)>
          func) override;

 private:
  std::vector<std::unique_ptr<PowerDistributionSimModel>> m_models;
};
}  // namespace

void PowerDistributionsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetPowerDistributionInitialized(i)) {
      if (!model) {
        model = std::make_unique<PowerDistributionSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void PowerDistributionsSimModel::ForEachPowerDistribution(
    wpi::function_ref<void(glass::PowerDistributionModel& model, int index)>
        func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool PowerDistributionsAnyInitialized() {
  static const int32_t num = HAL_GetNumREVPDHModules();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetPowerDistributionInitialized(i)) {
      return true;
    }
  }
  return false;
}

void PowerDistributionSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "PowerDistributions", PowerDistributionsAnyInitialized,
      [] { return std::make_unique<PowerDistributionsSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetDefaultPos(245, 155);
        return glass::MakeFunctionView([=] {
          DisplayPowerDistributions(
              static_cast<PowerDistributionsSimModel*>(model));
        });
      });
}
