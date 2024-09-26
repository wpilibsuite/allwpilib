// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AddressableLEDGui.h"

#include <memory>
#include <vector>

#include <glass/hardware/LEDDisplay.h>
#include <hal/Ports.h>
#include <hal/simulation/AddressableLEDData.h>
#include <imgui.h>

#include "HALSimGui.h"

using namespace halsimgui;

namespace {
class AddressableLEDModel : public glass::LEDDisplayModel {
 public:
  explicit AddressableLEDModel(int32_t index) : m_index{index} {}

  void Update() override {}
  bool Exists() override {
    return HALSIM_GetAddressableLEDInitialized(m_index);
  }

  bool IsRunning() override { return HALSIM_GetAddressableLEDRunning(m_index); }

  std::span<const Data> GetData(wpi::SmallVectorImpl<Data>&) override {
    size_t length = HALSIM_GetAddressableLEDData(m_index, m_data);
    return {reinterpret_cast<Data*>(m_data), length};
  }

 private:
  int32_t m_index;

  HAL_AddressableLEDData m_data[HAL_kAddressableLEDMaxLength];
};

class AddressableLEDsModel : public glass::LEDDisplaysModel {
 public:
  AddressableLEDsModel() : m_models(HAL_GetNumAddressableLEDs()) {}

  void Update() override;
  bool Exists() override;

  size_t GetNumLEDDisplays() override { return m_models.size(); }

  void ForEachLEDDisplay(
      wpi::function_ref<void(glass::LEDDisplayModel& model, int index)> func)
      override;

 private:
  std::vector<std::unique_ptr<AddressableLEDModel>> m_models;
};
}  // namespace

void AddressableLEDsModel::Update() {
  for (int i = 0; i < static_cast<int>(m_models.size()); ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetAddressableLEDInitialized(i)) {
      if (!model) {
        model = std::make_unique<AddressableLEDModel>(i);
      }
      if (model) {
        model->Update();
      }
    } else {
      model.reset();
    }
  }
}

bool AddressableLEDsModel::Exists() {
  for (auto&& model : m_models) {
    if (model && model->Exists()) {
      return true;
    }
  }
  return false;
}

void AddressableLEDsModel::ForEachLEDDisplay(
    wpi::function_ref<void(glass::LEDDisplayModel& model, int index)> func) {
  for (int i = 0; i < static_cast<int>(m_models.size()); ++i) {
    if (m_models[i]) {
      func(*m_models[i], i);
    }
  }
}

static bool AddressableLEDsExists() {
  static const int numLED = HAL_GetNumAddressableLEDs();
  for (int i = 0; i < numLED; ++i) {
    if (HALSIM_GetAddressableLEDInitialized(i)) {
      return true;
    }
  }
  return false;
}

void AddressableLEDGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Addressable LEDs", [] { return AddressableLEDsExists(); },
      [] { return std::make_unique<AddressableLEDsModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(290, 100);
        return glass::MakeFunctionView([=] {
          glass::DisplayLEDDisplays(static_cast<AddressableLEDsModel*>(model));
        });
      });
}
