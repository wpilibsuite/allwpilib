// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <glass/DataSource.h>
#include <glass/Model.h>
#include <hal/SimDevice.h>
#include <wpi/DenseMap.h>

namespace glass {
class DeviceTreeModel;
}  // namespace glass

namespace halsimgui {

class SimValueSource : public glass::DataSource {
 public:
  SimValueSource(HAL_SimValueHandle handle, const char* device,
                 const char* name);
  ~SimValueSource() override;

 private:
  static void CallbackFunc(const char*, void* param, HAL_SimValueHandle,
                           int32_t, const HAL_Value* value);

  int32_t m_callback;
};

class SimDevicesModel : public glass::Model {
 public:
  void Update() override;
  bool Exists() override { return true; }

  glass::DataSource* GetSource(HAL_SimValueHandle handle) {
    return m_sources[handle].get();
  }

 private:
  wpi::DenseMap<HAL_SimValueHandle, std::unique_ptr<SimValueSource>> m_sources;
};

void InitializeDeviceTree(glass::DeviceTreeModel& deviceTree,
                          SimDevicesModel* model);
void DisplayDeviceTree(glass::DeviceTreeModel* model);

}  // namespace halsimgui
