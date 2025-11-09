// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AddressableLEDGui.hpp"

#include <memory>
#include <vector>

#include <imgui.h>

#include "wpi/glass/hardware/LEDDisplay.hpp"
#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/AddressableLEDData.h"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
class AddressableLEDModel : public wpi::glass::LEDDisplayModel {
 public:
  explicit AddressableLEDModel(int32_t channel) : m_channel{channel} {}

  void Update() override {}
  bool Exists() override {
    return HALSIM_GetAddressableLEDInitialized(m_channel);
  }

  std::span<const Data> GetData(wpi::util::SmallVectorImpl<Data>&) override {
    size_t length = HALSIM_GetAddressableLEDData(
        HALSIM_GetAddressableLEDStart(m_channel),
        HALSIM_GetAddressableLEDLength(m_channel), m_data);
    return {reinterpret_cast<Data*>(m_data), length};
  }

 private:
  int32_t m_channel;

  HAL_AddressableLEDData m_data[HAL_kAddressableLEDMaxLength];
};

class AddressableLEDsModel : public wpi::glass::LEDDisplaysModel {
 public:
  AddressableLEDsModel() : m_models(HAL_GetNumAddressableLEDs()) {}

  void Update() override;
  bool Exists() override;

  size_t GetNumLEDDisplays() override { return m_models.size(); }

  void ForEachLEDDisplay(wpi::util::function_ref<
                         void(wpi::glass::LEDDisplayModel& model, int channel)>
                             func) override;

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
    wpi::util::function_ref<void(wpi::glass::LEDDisplayModel& model,
                                 int channel)>
        func) {
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
      [](wpi::glass::Window* win, wpi::glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(290, 100);
        return wpi::glass::MakeFunctionView([=] {
          wpi::glass::DisplayLEDDisplays(
              static_cast<AddressableLEDsModel*>(model));
        });
      });
}
