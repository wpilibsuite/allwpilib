// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PWMSimGui.h"

#include <memory>
#include <vector>

#include <glass/hardware/PWM.h>
#include <hal/Ports.h>
#include <hal/simulation/AddressableLEDData.h>
#include <hal/simulation/PWMData.h>

#include "HALDataSource.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PWMSpeed, "PWM");

class PWMSimModel : public glass::PWMModel {
 public:
  explicit PWMSimModel(int32_t index) : m_index{index}, m_speed{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetPWMInitialized(m_index); }

  void SetAddressableLED(int led) { m_led = led; }
  int GetAddressableLED() const override { return m_led; }

  glass::DataSource* GetSpeedData() override { return &m_speed; }

  void SetSpeed(double val) override { HALSIM_SetPWMSpeed(m_index, val); }

 private:
  int32_t m_index;
  int m_led = -1;
  PWMSpeedSource m_speed;
};

class PWMsSimModel : public glass::PWMsModel {
 public:
  PWMsSimModel() : m_sources(HAL_GetNumPWMChannels()) {}

  void Update() override;

  bool Exists() override;

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
      model->SetAddressableLED(-1);
    } else {
      model.reset();
    }
  }

  static const int32_t numLED = HAL_GetNumAddressableLEDs();
  for (int32_t i = 0; i < numLED; ++i) {
    if (HALSIM_GetAddressableLEDInitialized(i)) {
      int32_t channel = HALSIM_GetAddressableLEDOutputPort(i);
      if (channel >= 0 && channel < numPWM && m_sources[channel]) {
        m_sources[channel]->SetAddressableLED(i);
      }
    }
  }
}

bool PWMsSimModel::Exists() {
  for (auto&& model : m_sources) {
    if (model && model->GetAddressableLED() == -1) {
      return true;
    }
  }
  return false;
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

glass::PWMsModel* halsimgui::CreatePWMsModel() {
  return glass::CreateModel<PWMsSimModel>();
}
