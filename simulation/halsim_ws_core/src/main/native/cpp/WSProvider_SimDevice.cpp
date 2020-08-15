/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_SimDevice.h"

#include <hal/Ports.h>

namespace wpilibws {

void HALSimWSProviderSimDevice::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> ws) {
  auto storedWS = m_ws.lock();

  if (ws == storedWS) {
    return;
  }

  // Otherwise, run the disconnection code first so we get
  // back to a clean slate (only if the stored websocket is
  // not null)
  if (storedWS) {
    OnNetworkDisconnected();
  }

  m_ws = ws;

  m_simValueCreatedCbKey = HALSIM_RegisterSimValueCreatedCallback(
      m_handle, this, HALSimWSProviderSimDevice::OnValueCreatedStatic, 1);

  {
    auto it = m_valueHandles.begin();
    auto end = m_valueHandles.end();

    std::shared_lock lock(m_vhLock);
    HAL_Value value;
    for (; it != end; ++it) {
      HAL_GetSimValue(it->second->handle, &value);
      OnValueChanged(it->second.get(), &value);
    }
  }
}

void HALSimWSProviderSimDevice::OnNetworkDisconnected() {
  // Cancel all callbacks
  HALSIM_CancelSimValueCreatedCallback(m_simValueCreatedCbKey);

  for (auto& kv : m_simValueChangedCbKeys) {
    HALSIM_CancelSimValueChangedCallback(kv.getValue());
  }

  m_ws.reset();
}

void HALSimWSProviderSimDevice::OnNetValueChanged(const wpi::json& json) {
  auto it = json.cbegin();
  auto end = json.cend();

  std::shared_lock lock(m_vhLock);
  for (; it != end; ++it) {
    auto vd = m_valueHandles.find(it.key());
    if (vd != m_valueHandles.end()) {
      HAL_Value value;
      value.type = vd->second->valueType;
      switch (value.type) {
        case HAL_BOOLEAN:
          value.data.v_boolean = static_cast<bool>(it.value()) ? 1 : 0;
          break;
        case HAL_DOUBLE:
          value.data.v_double = it.value();
          break;
        case HAL_ENUM:
          value.data.v_enum = it.value();
          break;
        case HAL_INT:
          value.data.v_int = it.value();
          break;
        case HAL_LONG:
          value.data.v_long = it.value();
          break;
        default:
          break;
      }

      HAL_SetSimValue(vd->second->handle, &value);
    }
  }
}

void HALSimWSProviderSimDevice::OnValueCreated(const char* name,
                                               HAL_SimValueHandle handle,
                                               HAL_Bool readonly,
                                               const struct HAL_Value* value) {
  wpi::Twine key = wpi::Twine(readonly ? "<" : "<>") + name;
  auto data = std::make_unique<SimDeviceValueData>();
  data->device = this;
  data->handle = handle;
  data->key = key.str();
  data->valueType = value->type;

  auto param = data.get();

  {
    std::unique_lock lock(m_vhLock);
    m_valueHandles[data->key] = std::move(data);
  }

  int32_t cbKey = HALSIM_RegisterSimValueChangedCallback(
      handle, param, HALSimWSProviderSimDevice::OnValueChangedStatic, true);

  m_simValueChangedCbKeys[key.str()] = cbKey;
}

void HALSimWSProviderSimDevice::OnValueChanged(SimDeviceValueData* valueData,
                                               const struct HAL_Value* value) {
  auto ws = m_ws.lock();
  if (ws) {
    switch (value->type) {
      case HAL_BOOLEAN:
        ProcessHalCallback({{valueData->key, value->data.v_boolean}});
        break;
      case HAL_DOUBLE:
        ProcessHalCallback({{valueData->key, value->data.v_double}});
        break;
      case HAL_ENUM:
        ProcessHalCallback({{valueData->key, value->data.v_enum}});
        break;
      case HAL_INT:
        ProcessHalCallback({{valueData->key, value->data.v_int}});
        break;
      case HAL_LONG:
        ProcessHalCallback({{valueData->key, value->data.v_long}});
        break;
      default:
        break;
    }
  }
}

void HALSimWSProviderSimDevice::ProcessHalCallback(const wpi::json& payload) {
  auto ws = m_ws.lock();
  if (ws) {
    wpi::json netValue = {
        {"type", "SimDevices"}, {"device", m_deviceId}, {"data", payload}};
    ws->OnSimValueChanged(netValue);
  }
}

void HALSimWSProviderSimDevices::DeviceCreatedCallback(
    const char* name, HAL_SimDeviceHandle handle) {
  auto key = (wpi::Twine("SimDevices/") + name).str();
  auto dev = std::make_shared<HALSimWSProviderSimDevice>(
      handle, key, wpi::Twine(name).str());
  m_providers.Add(key, dev);

  if (m_ws) {
    m_exec->Call([this, dev]() { dev->OnNetworkConnected(GetWSConnection()); });
  }
}

void HALSimWSProviderSimDevices::DeviceFreedCallback(
    const char* name, HAL_SimDeviceHandle handle) {
  m_providers.Delete(name);
}

void HALSimWSProviderSimDevices::Initialize(
    std::shared_ptr<wpi::uv::Loop> loop) {
  HALSIM_RegisterSimDeviceCreatedCallback(
      "", this, HALSimWSProviderSimDevices::DeviceCreatedCallbackStatic, 1);
  HALSIM_RegisterSimDeviceFreedCallback(
      "", this, HALSimWSProviderSimDevices::DeviceFreedCallbackStatic);

  m_exec = UvExecFn::Create(loop, [](auto out, LoopFn func) {
    func();
    out.set_value();
  });
}

void HALSimWSProviderSimDevices::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  m_ws = hws;
}

void HALSimWSProviderSimDevices::OnNetworkDisconnected() { m_ws = nullptr; }

}  // namespace wpilibws
