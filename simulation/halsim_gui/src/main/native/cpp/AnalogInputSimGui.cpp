// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogInputSimGui.hpp"

#include <memory>
#include <vector>

#include "wpi/glass/View.hpp"
#include "wpi/glass/hardware/AnalogInput.hpp"
#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/AnalogInData.h"
#include "wpi/hal/simulation/SimDeviceData.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogInVoltage, "AIn");

class AnalogInputSimModel : public glass::AnalogInputModel {
 public:
  explicit AnalogInputSimModel(int32_t index)
      : m_index{index}, m_voltageData{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetAnalogInInitialized(m_index); }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetAnalogInSimDevice(m_index)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  glass::DoubleSource* GetVoltageData() override { return &m_voltageData; }

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
