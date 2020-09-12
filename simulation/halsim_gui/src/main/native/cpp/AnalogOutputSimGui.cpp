/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutputSimGui.h"

#include <glass/hardware/AnalogOutput.h>
#include <glass/other/DeviceTree.h>

#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/AnalogOutData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogOutVoltage, "AOut");

class AnalogOutputSimModel : public glass::AnalogOutputModel {
 public:
  explicit AnalogOutputSimModel(int32_t index)
      : m_index{index}, m_voltageData{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetAnalogOutInitialized(m_index); }

  glass::DataSource* GetVoltageData() override { return &m_voltageData; }

  void SetVoltage(double val) override {
    HALSIM_SetAnalogOutVoltage(m_index, val);
  }

 private:
  int32_t m_index;
  AnalogOutVoltageSource m_voltageData;
};

class AnalogOutputsSimModel : public glass::AnalogOutputsModel {
 public:
  AnalogOutputsSimModel() : m_models(HAL_GetNumAnalogOutputs()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachAnalogOutput(
      wpi::function_ref<void(glass::AnalogOutputModel& model, int index)> func)
      override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<AnalogOutputSimModel>> m_models;
};
}  // namespace

void AnalogOutputsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetAnalogOutInitialized(i)) {
      if (!model) {
        model = std::make_unique<AnalogOutputSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void AnalogOutputsSimModel::ForEachAnalogOutput(
    wpi::function_ref<void(glass::AnalogOutputModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

void AnalogOutputSimGui::Initialize() {
  SimDeviceGui::GetDeviceTree().Add(
      std::make_unique<AnalogOutputsSimModel>(), [](glass::Model* model) {
        glass::DisplayAnalogOutputsDevice(
            static_cast<AnalogOutputsSimModel*>(model));
      });
}
