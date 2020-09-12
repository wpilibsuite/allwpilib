/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PDPSimGui.h"

#include <glass/hardware/PDP.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/PDPData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PDPTemperature, "PDP Temp");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PDPVoltage, "PDP Voltage");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED2(PDPCurrent, "PDP Current");

class PDPSimModel : public glass::PDPModel {
 public:
  explicit PDPSimModel(int32_t index)
      : m_index{index}, m_temp{index}, m_voltage{index} {
    const int numChannels = HAL_GetNumPDPChannels();
    m_currents.reserve(numChannels);
    for (int i = 0; i < numChannels; ++i)
      m_currents.emplace_back(std::make_unique<PDPCurrentSource>(index, i));
  }

  void Update() override {}

  bool Exists() override { return HALSIM_GetPDPInitialized(m_index); }

  int GetNumChannels() const override { return m_currents.size(); }

  glass::DataSource* GetTemperatureData() override { return &m_temp; }
  glass::DataSource* GetVoltageData() override { return &m_voltage; }
  glass::DataSource* GetCurrentData(int channel) override {
    return m_currents[channel].get();
  }

  void SetTemperature(double val) override {
    HALSIM_SetPDPTemperature(m_index, val);
  }
  void SetVoltage(double val) override { HALSIM_SetPDPVoltage(m_index, val); }
  void SetCurrent(int channel, double val) override {
    HALSIM_SetPDPCurrent(m_index, channel, val);
  }

 private:
  int32_t m_index;
  PDPTemperatureSource m_temp;
  PDPVoltageSource m_voltage;
  std::vector<std::unique_ptr<PDPCurrentSource>> m_currents;
};

class PDPsSimModel : public glass::PDPsModel {
 public:
  PDPsSimModel() : m_models(HAL_GetNumPDPModules()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPDP(
      wpi::function_ref<void(glass::PDPModel& model, int index)> func) override;

 private:
  std::vector<std::unique_ptr<PDPSimModel>> m_models;
};
}  // namespace

void PDPsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetPDPInitialized(i)) {
      if (!model) {
        model = std::make_unique<PDPSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void PDPsSimModel::ForEachPDP(
    wpi::function_ref<void(glass::PDPModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool PDPsAnyInitialized() {
  static const int32_t num = HAL_GetNumPDPModules();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetPDPInitialized(i)) return true;
  }
  return false;
}

void PDPSimGui::Initialize() {
  HALSimGui::halProvider.Register(
      "PDPs", PDPsAnyInitialized,
      [] { return std::make_unique<PDPsSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetDefaultPos(245, 155);
        return glass::MakeFunctionView(
            [=] { DisplayPDPs(static_cast<PDPsSimModel*>(model)); });
      });
}
