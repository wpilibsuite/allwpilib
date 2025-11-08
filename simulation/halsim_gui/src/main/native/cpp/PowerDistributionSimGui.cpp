// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PowerDistributionSimGui.hpp"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "wpi/glass/hardware/PowerDistribution.hpp"
#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/PowerDistributionData.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PowerDistributionTemperature,
                                    "Power Distribution Temp");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PowerDistributionVoltage,
                                    "Power Distribution Voltage");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED2(PowerDistributionCurrent,
                                     "Power Distribution Current");

class PowerDistributionSimModel : public wpi::glass::PowerDistributionModel {
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

  wpi::glass::DoubleSource* GetTemperatureData() override { return &m_temp; }
  wpi::glass::DoubleSource* GetVoltageData() override { return &m_voltage; }
  wpi::glass::DoubleSource* GetCurrentData(int channel) override {
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

class PowerDistributionsSimModel : public wpi::glass::PowerDistributionsModel {
 public:
  PowerDistributionsSimModel() : m_models(HAL_GetNumREVPDHModules()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPowerDistribution(
      wpi::util::function_ref<void(wpi::glass::PowerDistributionModel& model, int index)>
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
    wpi::util::function_ref<void(wpi::glass::PowerDistributionModel& model, int index)>
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
      [](wpi::glass::Window* win, wpi::glass::Model* model) {
        win->SetDefaultPos(245, 155);
        return wpi::glass::MakeFunctionView([=] {
          DisplayPowerDistributions(
              static_cast<PowerDistributionsSimModel*>(model));
        });
      });
}
