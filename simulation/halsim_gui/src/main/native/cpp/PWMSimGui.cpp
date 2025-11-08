// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PWMSimGui.hpp"

#include <memory>
#include <vector>

#include "wpi/glass/hardware/PWM.hpp"
#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/AddressableLEDData.h"
#include "wpi/hal/simulation/PWMData.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PWMPulseMicrosecond, "PWM");

class PWMSimModel : public glass::PWMModel {
 public:
  explicit PWMSimModel(int32_t index) : m_index{index}, m_speed{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetPWMInitialized(m_index); }

  glass::DoubleSource* GetSpeedData() override { return &m_speed; }

  void SetSpeed(double val) override {
    HALSIM_SetPWMPulseMicrosecond(m_index, val);
  }

 private:
  int32_t m_index;
  PWMPulseMicrosecondSource m_speed;
};

class PWMsSimModel : public glass::PWMsModel {
 public:
  PWMsSimModel() : m_sources(HAL_GetNumPWMChannels()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachPWM(
      wpi::function_ref<void(glass::PWMModel& model, int index)> func) override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<PWMSimModel>> m_sources;
};
}  // namespace

void PWMsSimModel::Update() {
  const int32_t numPWM = m_sources.size();
  for (int32_t i = 0; i < numPWM; ++i) {
    auto& model = m_sources[i];
    if (HALSIM_GetPWMInitialized(i)) {
      if (!model) {
        model = std::make_unique<PWMSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void PWMsSimModel::ForEachPWM(
    wpi::function_ref<void(glass::PWMModel& model, int index)> func) {
  const int32_t numPWM = m_sources.size();
  for (int32_t i = 0; i < numPWM; ++i) {
    if (auto model = m_sources[i].get()) {
      func(*model, i);
    }
  }
}

static bool PWMsAnyInitialized() {
  static const int32_t num = HAL_GetNumPWMChannels();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetPWMInitialized(i)) {
      return true;
    }
  }
  return false;
}

void PWMSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "PWM Outputs", PWMsAnyInitialized,
      [] { return std::make_unique<PWMsSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(910, 20);
        return glass::MakeFunctionView([=] {
          glass::DisplayPWMs(static_cast<PWMsSimModel*>(model),
                             HALSimGui::halProvider->AreOutputsEnabled());
        });
      });
}
