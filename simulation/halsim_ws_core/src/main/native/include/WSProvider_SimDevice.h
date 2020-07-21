/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <wpi/StringMap.h>

#include "WSBaseProvider.h"
#include "WSProviderContainer.h"

namespace wpilibws {

class HALSimWSProviderSimDevice;

struct SimDeviceValueData {
  HALSimWSProviderSimDevice* device;
  HAL_SimValueHandle handle;
  std::string key;
  HAL_Type valueType;
};

class HALSimWSProviderSimDevice : public HALSimWSBaseProvider {
 public:
  HALSimWSProviderSimDevice(HAL_SimDeviceHandle handle, const std::string& key,
                            const std::string& deviceId)
      : HALSimWSBaseProvider(key, "SimDevices"), m_handle(handle) {
    m_deviceId = deviceId;
  }

  void Initialize();

  void OnNetworkConnected(
      std::shared_ptr<HALSimBaseWebSocketConnection> ws) override;
  void OnNetValueChanged(const wpi::json& json) override;

 private:
  static void OnValueCreatedStatic(const char* name, void* param,
                                   HAL_SimValueHandle handle, HAL_Bool readonly,
                                   const struct HAL_Value* value) {
    (reinterpret_cast<HALSimWSProviderSimDevice*>(param))
        ->OnValueCreated(name, handle, readonly, value);
  }
  void OnValueCreated(const char* name, HAL_SimValueHandle handle,
                      HAL_Bool readonly, const struct HAL_Value* value);

  static void OnValueChangedStatic(const char* name, void* param,
                                   HAL_SimValueHandle handle, HAL_Bool readonly,
                                   const struct HAL_Value* value) {
    auto valueData = (reinterpret_cast<SimDeviceValueData*>(param));
    valueData->device->OnValueChanged(valueData, value);
  }
  void OnValueChanged(SimDeviceValueData* valueData,
                      const struct HAL_Value* value);

  wpi::StringMap<std::unique_ptr<SimDeviceValueData>> m_valueHandles;
  std::shared_mutex m_vhLock;

  HAL_SimDeviceHandle m_handle;
};

class HALSimWSProviderSimDevices {
 public:
  explicit HALSimWSProviderSimDevices(ProviderContainer& providers)
      : m_providers(providers) {}
  void Initialize();

 private:
  static void DeviceCreatedCallbackStatic(const char* name, void* param,
                                          HAL_SimDeviceHandle handle) {
    (reinterpret_cast<HALSimWSProviderSimDevices*>(param))
        ->DeviceCreatedCallback(name, handle);
  }
  void DeviceCreatedCallback(const char* name, HAL_SimDeviceHandle handle);

  static void DeviceFreedCallbackStatic(const char* name, void* param,
                                        HAL_SimDeviceHandle handle) {
    (reinterpret_cast<HALSimWSProviderSimDevices*>(param))
        ->DeviceFreedCallback(name, handle);
  }
  void DeviceFreedCallback(const char* name, HAL_SimDeviceHandle handle);

  ProviderContainer& m_providers;
};

}  // namespace wpilibws
