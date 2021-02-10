// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <wpi/StringMap.h>
#include <wpi/uv/AsyncFunction.h>

#include "WSBaseProvider.h"
#include "WSProviderContainer.h"

namespace wpilibws {

class HALSimWSProviderSimDevice;
class HALSimWSProviderSimDevices;

struct SimDeviceValueData {
  HALSimWSProviderSimDevice* device;
  HAL_SimValueHandle handle;
  std::string key;
  std::vector<std::string> options;
  std::vector<double> optionValues;
  HAL_Type valueType;
  double doubleOffset = 0;
  int64_t intOffset = 0;
};

class HALSimWSProviderSimDevice : public HALSimWSBaseProvider {
 public:
  HALSimWSProviderSimDevice(HAL_SimDeviceHandle handle, const std::string& key,
                            const std::string& type,
                            const std::string& deviceId)
      : HALSimWSBaseProvider(key, type), m_handle(handle) {
    m_deviceId = deviceId;
  }

  ~HALSimWSProviderSimDevice() override;

  void OnNetworkConnected(
      std::shared_ptr<HALSimBaseWebSocketConnection> ws) override;

  void OnNetworkDisconnected() override;

  void OnNetValueChanged(const wpi::json& json) override;

  void ProcessHalCallback(const wpi::json& payload);

 private:
  static void OnValueCreatedStatic(const char* name, void* param,
                                   HAL_SimValueHandle handle, int32_t direction,
                                   const struct HAL_Value* value);
  void OnValueCreated(const char* name, HAL_SimValueHandle handle,
                      int32_t direction, const struct HAL_Value* value);

  static void OnValueChangedStatic(const char* name, void* param,
                                   HAL_SimValueHandle handle, int32_t direction,
                                   const struct HAL_Value* value);
  void OnValueChanged(SimDeviceValueData* valueData,
                      const struct HAL_Value* value);

  static void OnValueResetStatic(const char* name, void* param,
                                 HAL_SimValueHandle handle, int32_t direction,
                                 const struct HAL_Value* value);
  void OnValueReset(SimDeviceValueData* valueData,
                    const struct HAL_Value* value);

  void CancelCallbacks();

  wpi::StringMap<std::unique_ptr<SimDeviceValueData>> m_valueHandles;
  std::shared_mutex m_vhLock;

  HAL_SimDeviceHandle m_handle;

  std::shared_ptr<HALSimWSProviderSimDevices> m_simDevicesBase;

  int32_t m_simValueCreatedCbKey = 0;
  wpi::StringMap<int32_t> m_simValueChangedCbKeys;
};

class HALSimWSProviderSimDevices {
 public:
  using LoopFn = std::function<void(void)>;
  using UvExecFn = wpi::uv::AsyncFunction<void(LoopFn)>;

  explicit HALSimWSProviderSimDevices(ProviderContainer& providers)
      : m_providers(providers) {}
  ~HALSimWSProviderSimDevices();

  void Initialize(wpi::uv::Loop& loop);

  void OnNetworkConnected(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void OnNetworkDisconnected();

  std::shared_ptr<HALSimBaseWebSocketConnection> GetWSConnection() {
    return m_ws;
  }

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

  void CancelCallbacks();

  ProviderContainer& m_providers;

  std::shared_ptr<HALSimBaseWebSocketConnection> m_ws;
  std::shared_ptr<UvExecFn> m_exec;

  int32_t m_deviceCreatedCbKey = 0;
  int32_t m_deviceFreedCbKey = 0;
};

}  // namespace wpilibws
