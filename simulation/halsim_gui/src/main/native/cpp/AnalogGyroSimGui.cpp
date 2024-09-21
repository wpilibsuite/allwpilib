// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogGyroSimGui.h"

#include <memory>
#include <vector>

#include <glass/hardware/AnalogGyro.h>
#include <glass/other/DeviceTree.h>
#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/AnalogGyroData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogGyroAngle, "AGyro Angle");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogGyroRate, "AGyro Rate");

class AnalogGyroSimModel : public glass::AnalogGyroModel {
 public:
  explicit AnalogGyroSimModel(int32_t index)
      : m_index{index}, m_angle{index}, m_rate{index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetAnalogGyroInitialized(m_index); }

  glass::DataSource* GetAngleData() override { return &m_angle; }
  glass::DataSource* GetRateData() override { return &m_rate; }

  void SetAngle(double val) override {
    HALSIM_SetAnalogGyroAngle(m_index, val);
  }
  void SetRate(double val) override { HALSIM_SetAnalogGyroRate(m_index, val); }

 private:
  int32_t m_index;
  AnalogGyroAngleSource m_angle;
  AnalogGyroRateSource m_rate;
};

class AnalogGyrosSimModel : public glass::AnalogGyrosModel {
 public:
  AnalogGyrosSimModel() : m_models(HAL_GetNumAccumulators()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachAnalogGyro(
      wpi::function_ref<void(glass::AnalogGyroModel& model, int index)> func)
      override;

 private:
  // indexed by channel
  std::vector<std::unique_ptr<AnalogGyroSimModel>> m_models;
};
}  // namespace

void AnalogGyrosSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetAnalogGyroInitialized(i)) {
      if (!model) {
        model = std::make_unique<AnalogGyroSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void AnalogGyrosSimModel::ForEachAnalogGyro(
    wpi::function_ref<void(glass::AnalogGyroModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

void AnalogGyroSimGui::Initialize() {
  SimDeviceGui::GetDeviceTree().Add(
      std::make_unique<AnalogGyrosSimModel>(), [](glass::Model* model) {
        glass::DisplayAnalogGyrosDevice(
            static_cast<AnalogGyrosSimModel*>(model));
      });
}
