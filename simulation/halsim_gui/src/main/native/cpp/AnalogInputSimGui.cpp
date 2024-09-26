// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogInputSimGui.h"

#include <memory>
#include <vector>

#include <glass/View.h>
#include <glass/hardware/AnalogInput.h>
#include <hal/Ports.h>
#include <hal/simulation/AnalogGyroData.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/SimDeviceData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogInVoltage, "AIn");

class AnalogInputSimModel : public glass::AnalogInputModel {
 public:
  explicit AnalogInputSimModel(int32_t index)
      : m_index{index}, m_voltageData{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetAnalogInInitialized(m_index); }

  bool IsGyro() const override {
    return m_index < HAL_GetNumAccumulators() &&
           HALSIM_GetAnalogGyroInitialized(m_index);
  }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetAnalogInSimDevice(m_index)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  glass::DataSource* GetVoltageData() override { return &m_voltageData; }

  void SetVoltage(double val) override {
    HALSIM_SetAnalogInVoltage(m_index, val);
  }

 private:
  int32_t m_index;
  AnalogInVoltageSource m_voltageData;
};

class AnalogInputsSimModel : public glass::AnalogInputsModel {
 public:
  AnalogInputsSimModel() : m_models(HAL_GetNumAnalogInputs()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachAnalogInput(
      wpi::function_ref<void(glass::AnalogInputModel& model, int index)> func)
      override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<AnalogInputSimModel>> m_models;
};
}  // namespace

void AnalogInputsSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetAnalogInInitialized(i)) {
      if (!model) {
        model = std::make_unique<AnalogInputSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void AnalogInputsSimModel::ForEachAnalogInput(
    wpi::function_ref<void(glass::AnalogInputModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool AnalogInputsAnyInitialized() {
  static const int32_t num = HAL_GetNumAnalogInputs();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetAnalogInInitialized(i)) {
      return true;
    }
  }
  return false;
}

void AnalogInputSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Analog Inputs", AnalogInputsAnyInitialized,
      [] { return std::make_unique<AnalogInputsSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(640, 20);
        return glass::MakeFunctionView([=] {
          glass::DisplayAnalogInputs(static_cast<AnalogInputsSimModel*>(model));
        });
      });
}
