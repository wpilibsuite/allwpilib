// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AccelerometerSimGui.h"

#include <memory>

#include <glass/hardware/Accelerometer.h>
#include <glass/other/DeviceTree.h>
#include <hal/Value.h>
#include <hal/simulation/AccelerometerData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace glass;
using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerX, "X Accel");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerY, "Y Accel");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerZ, "Z Accel");

class AccelerometerSimModel : public glass::AccelerometerModel {
 public:
  explicit AccelerometerSimModel(int32_t index)
      : m_index{index}, m_xData{m_index}, m_yData{m_index}, m_zData{m_index} {}

  void Update() override {}

  bool Exists() override { return HALSIM_GetAccelerometerActive(m_index); }

  glass::DataSource* GetXData() override { return &m_xData; }
  glass::DataSource* GetYData() override { return &m_yData; }
  glass::DataSource* GetZData() override { return &m_zData; }

  int GetRange() override { return HALSIM_GetAccelerometerRange(m_index); }

  void SetX(double val) override { HALSIM_SetAccelerometerX(m_index, val); }
  void SetY(double val) override { HALSIM_SetAccelerometerY(m_index, val); }
  void SetZ(double val) override { HALSIM_SetAccelerometerZ(m_index, val); }
  void SetRange(int val) override {
    HALSIM_SetAccelerometerRange(m_index,
                                 static_cast<HAL_AccelerometerRange>(val));
  }

 private:
  int32_t m_index;
  AccelerometerXSource m_xData;
  AccelerometerYSource m_yData;
  AccelerometerZSource m_zData;
};
}  // namespace

void AccelerometerSimGui::Initialize() {
  SimDeviceGui::GetDeviceTree().Add(
      std::make_unique<AccelerometerSimModel>(0), [](glass::Model* model) {
        glass::DisplayAccelerometerDevice(
            static_cast<AccelerometerSimModel*>(model));
      });
}
