// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RelaySimGui.h"

#include <memory>
#include <vector>

#include <glass/hardware/Relay.h>
#include <hal/Ports.h>
#include <hal/simulation/RelayData.h>
#include <wpigui.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(RelayForward, "RelayFwd");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(RelayReverse, "RelayRev");

class RelaySimModel : public glass::RelayModel {
 public:
  explicit RelaySimModel(int32_t index)
      : m_index{index}, m_forward{index}, m_reverse{index} {}

  void Update() override {}

  bool Exists() override {
    return HALSIM_GetRelayInitializedForward(m_index) ||
           HALSIM_GetRelayInitializedReverse(m_index);
  }

  glass::DataSource* GetForwardData() override {
    return HALSIM_GetRelayInitializedForward(m_index) ? &m_forward : nullptr;
  }
  glass::DataSource* GetReverseData() override {
    return HALSIM_GetRelayInitializedReverse(m_index) ? &m_reverse : nullptr;
  }

  void SetForward(bool val) override { HALSIM_SetRelayForward(m_index, val); }
  void SetReverse(bool val) override { HALSIM_SetRelayReverse(m_index, val); }

 private:
  int32_t m_index;
  RelayForwardSource m_forward;
  RelayReverseSource m_reverse;
};

class RelaysSimModel : public glass::RelaysModel {
 public:
  RelaysSimModel() : m_models(HAL_GetNumRelayHeaders()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachRelay(wpi::function_ref<void(glass::RelayModel& model, int index)>
                        func) override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<RelaySimModel>> m_models;
};
}  // namespace

void RelaysSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetRelayInitializedForward(i) ||
        HALSIM_GetRelayInitializedReverse(i)) {
      if (!model) {
        model = std::make_unique<RelaySimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void RelaysSimModel::ForEachRelay(
    wpi::function_ref<void(glass::RelayModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool RelayAnyInitialized() {
  static const int32_t num = HAL_GetNumRelayHeaders();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetRelayInitializedForward(i) ||
        HALSIM_GetRelayInitializedReverse(i)) {
      return true;
    }
  }
  return false;
}

void RelaySimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Relays", RelayAnyInitialized,
      [] { return std::make_unique<RelaysSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(180, 20);
        return glass::MakeFunctionView([=] {
          glass::DisplayRelays(static_cast<RelaysSimModel*>(model),
                               HALSimGui::halProvider->AreOutputsEnabled());
        });
      });
}
