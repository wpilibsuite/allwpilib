// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DIOSimGui.hpp"

#include <memory>
#include <vector>

#include "EncoderSimGui.hpp"
#include "wpi/glass/hardware/DIO.hpp"
#include "wpi/glass/hardware/Encoder.hpp"
#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/DIOData.h"
#include "wpi/hal/simulation/DigitalPWMData.h"
#include "wpi/hal/simulation/DutyCycleData.h"
#include "wpi/hal/simulation/EncoderData.h"
#include "wpi/hal/simulation/SimDeviceData.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(DIOValue, "DIO");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(DigitalPWMDutyCycle, "DPWM");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(DutyCycleOutput, "DutyCycle");

class DPWMSimModel : public wpi::glass::DPWMModel {
 public:
  DPWMSimModel(int32_t index, int32_t dioChannel)
      : m_dioChannel{dioChannel}, m_index{index}, m_valueData{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetDigitalPWMInitialized(m_index); }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetDIOSimDevice(m_dioChannel)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  wpi::glass::DoubleSource* GetValueData() override { return &m_valueData; }

  void SetValue(double val) override {
    HALSIM_SetDigitalPWMDutyCycle(m_index, val);
  }

 private:
  int32_t m_dioChannel;
  int32_t m_index;
  DigitalPWMDutyCycleSource m_valueData;
};

class DutyCycleSimModel : public wpi::glass::DutyCycleModel {
 public:
  explicit DutyCycleSimModel(int32_t index)
      : m_index{index}, m_valueData{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetDutyCycleInitialized(m_index); }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetDutyCycleSimDevice(m_index)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  wpi::glass::DoubleSource* GetValueData() override { return &m_valueData; }

  void SetValue(double val) override {
    HALSIM_SetDutyCycleOutput(m_index, val);
  }

 private:
  int32_t m_index;
  DutyCycleOutputSource m_valueData;
};

class DIOSimModel : public wpi::glass::DIOModel {
 public:
  explicit DIOSimModel(int32_t channel)
      : m_channel{channel}, m_valueData{channel} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetDIOInitialized(m_channel); }

  bool IsReadOnly() override { return !IsInput(); }

  const char* GetName() const override { return ""; }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetDIOSimDevice(m_channel)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  DPWMSimModel* GetDPWM() override { return m_dpwmSource; }
  DutyCycleSimModel* GetDutyCycle() override { return m_dutyCycleSource; }
  wpi::glass::EncoderModel* GetEncoder() override { return m_encoderSource; }

  void SetDPWM(DPWMSimModel* model) { m_dpwmSource = model; }
  void SetDutyCycle(DutyCycleSimModel* model) { m_dutyCycleSource = model; }
  void SetEncoder(wpi::glass::EncoderModel* model) { m_encoderSource = model; }

  bool IsInput() const override { return HALSIM_GetDIOIsInput(m_channel); }

  wpi::glass::BooleanSource* GetValueData() override { return &m_valueData; }

  void SetValue(bool val) override { HALSIM_SetDIOValue(m_channel, val); }

 private:
  int32_t m_channel;
  DIOValueSource m_valueData;
  DPWMSimModel* m_dpwmSource = nullptr;
  DutyCycleSimModel* m_dutyCycleSource = nullptr;
  wpi::glass::EncoderModel* m_encoderSource = nullptr;
};

class DIOsSimModel : public wpi::glass::DIOsModel {
 public:
  DIOsSimModel()
      : m_dioModels(HAL_GetNumDigitalChannels()),
        m_dpwmModels(HAL_GetNumDigitalPWMOutputs()),
        m_dutyCycleModels(HAL_GetNumDutyCycles()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachDIO(
      wpi::util::function_ref<void(wpi::glass::DIOModel& model, int index)> func) override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<DIOSimModel>> m_dioModels;
  // indexed by index
  std::vector<std::unique_ptr<DPWMSimModel>> m_dpwmModels;
  std::vector<std::unique_ptr<DutyCycleSimModel>> m_dutyCycleModels;
};
}  // namespace

void DIOsSimModel::Update() {
  const int32_t numDIO = m_dioModels.size();
  for (int i = 0; i < numDIO; ++i) {
    auto& model = m_dioModels[i];
    if (HALSIM_GetDIOInitialized(i)) {
      if (!model) {
        model = std::make_unique<DIOSimModel>(i);
      }
      model->SetDPWM(nullptr);
      model->SetDutyCycle(nullptr);
      model->SetEncoder(nullptr);
    } else {
      model.reset();
    }
  }

  const int32_t numPWM = m_dpwmModels.size();
  for (int32_t i = 0; i < numPWM; ++i) {
    auto& model = m_dpwmModels[i];
    if (HALSIM_GetDigitalPWMInitialized(i)) {
      if (!model) {
        int channel = HALSIM_GetDigitalPWMPin(i);
        if (channel >= 0 && channel < numDIO && m_dioModels[channel]) {
          model = std::make_unique<DPWMSimModel>(i, channel);
          m_dioModels[channel]->SetDPWM(model.get());
        }
      }
    } else {
      model.reset();
    }
  }

  const int32_t numDutyCycle = m_dutyCycleModels.size();
  for (int32_t i = 0; i < numDutyCycle; ++i) {
    auto& model = m_dutyCycleModels[i];
    if (HALSIM_GetDutyCycleInitialized(i)) {
      if (!model) {
        int channel = i;
        if (channel >= 0 && channel < numDIO && m_dioModels[channel]) {
          model = std::make_unique<DutyCycleSimModel>(i);
          m_dioModels[channel]->SetDutyCycle(model.get());
        }
      }
    } else {
      model.reset();
    }
  }

  EncoderSimGui::GetEncodersModel().ForEachEncoder([&](auto& encoder, int i) {
    int channel = encoder.GetChannelA();
    if (channel >= 0 && channel < numDIO && m_dioModels[channel]) {
      m_dioModels[channel]->SetEncoder(&encoder);
    }
    channel = encoder.GetChannelB();
    if (channel >= 0 && channel < numDIO && m_dioModels[channel]) {
      m_dioModels[channel]->SetEncoder(&encoder);
    }
  });
}

void DIOsSimModel::ForEachDIO(
    wpi::util::function_ref<void(wpi::glass::DIOModel& model, int index)> func) {
  const int32_t numDIO = m_dioModels.size();
  for (int32_t i = 0; i < numDIO; ++i) {
    if (auto model = m_dioModels[i].get()) {
      func(*model, i);
    }
  }
}

static bool DIOAnyInitialized() {
  static const int32_t num = HAL_GetNumDigitalChannels();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetDIOInitialized(i)) {
      return true;
    }
  }
  return false;
}

void DIOSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "DIO", DIOAnyInitialized, [] { return std::make_unique<DIOsSimModel>(); },
      [](wpi::glass::Window* win, wpi::glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(470, 20);
        return wpi::glass::MakeFunctionView([=] {
          wpi::glass::DisplayDIOs(static_cast<DIOsSimModel*>(model),
                             HALSimGui::halProvider->AreOutputsEnabled());
        });
      });
}
