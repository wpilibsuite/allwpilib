// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_SimDevice.h"

#include <algorithm>
#include <cmath>

#include <hal/Ports.h>

namespace wpilibws {

HALSimWSProviderSimDevice::~HALSimWSProviderSimDevice() {
  CancelCallbacks();
}

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
}

void HALSimWSProviderSimDevice::OnNetworkDisconnected() {
  // Cancel all callbacks
  CancelCallbacks();

  m_ws.reset();
}

void HALSimWSProviderSimDevice::CancelCallbacks() {
  HALSIM_CancelSimValueCreatedCallback(m_simValueCreatedCbKey);

  m_simValueCreatedCbKey = 0;

  for (auto& kv : m_simValueChangedCbKeys) {
    HALSIM_CancelSimValueChangedCallback(kv.getValue());
  }

  m_simValueChangedCbKeys.clear();
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
          value.data.v_double -= vd->second->doubleOffset;
          break;
        case HAL_ENUM: {
          if (it->is_string()) {
            auto& options = vd->second->options;
            auto& str = it.value().get_ref<const std::string&>();
            auto optionIt =
                std::find_if(options.begin(), options.end(),
                             [&](const std::string& v) { return v == str; });
            if (optionIt != options.end()) {
              value.data.v_enum = optionIt - options.begin();
            }
          } else if (it->is_number()) {
            auto& values = vd->second->optionValues;
            double num = it.value();
            auto valueIt = std::find_if(
                values.begin(), values.end(),
                [&](double v) { return std::fabs(v - num) < 1e-4; });
            if (valueIt != values.end()) {
              value.data.v_enum = valueIt - values.begin();
            }
          }
          value.data.v_enum = it.value();
          break;
        }
        case HAL_INT:
          value.data.v_int = it.value();
          value.data.v_int -= vd->second->intOffset;
          break;
        case HAL_LONG:
          value.data.v_long = it.value();
          value.data.v_long -= vd->second->intOffset;
          break;
        default:
          break;
      }

      HAL_SetSimValue(vd->second->handle, &value);
    }
  }
}

void HALSimWSProviderSimDevice::OnValueCreatedStatic(
    const char* name, void* param, HAL_SimValueHandle handle, int32_t direction,
    const struct HAL_Value* value) {
  (reinterpret_cast<HALSimWSProviderSimDevice*>(param))
      ->OnValueCreated(name, handle, direction, value);
}

void HALSimWSProviderSimDevice::OnValueCreated(const char* name,
                                               HAL_SimValueHandle handle,
                                               int32_t direction,
                                               const struct HAL_Value* value) {
  const char* prefix = "";
  if (name[0] != '<' && name[0] != '>') {
    switch (direction) {
      case HAL_SimValueInput:
        prefix = ">";
        break;
      case HAL_SimValueOutput:
        prefix = "<";
        break;
      case HAL_SimValueBidir:
        prefix = "<>";
        break;
      default:
        break;
    }
  }
  std::string key = (wpi::Twine(prefix) + name).str();
  auto data = std::make_unique<SimDeviceValueData>();
  data->device = this;
  data->handle = handle;
  data->key = key;
  if (value->type == HAL_ENUM) {
    int32_t numOptions = 0;

    const char** options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);
    data->options.reserve(numOptions);
    for (int32_t i = 0; i < numOptions; ++i) {
      data->options.emplace_back(options[i]);
    }

    const double* values =
        HALSIM_GetSimValueEnumDoubleValues(handle, &numOptions);
    data->optionValues.assign(values, values + numOptions);
  }
  data->valueType = value->type;

  auto param = data.get();

  {
    std::unique_lock lock(m_vhLock);
    m_valueHandles[data->key] = std::move(data);
  }

  int32_t cbKey = HALSIM_RegisterSimValueChangedCallback(
      handle, param, HALSimWSProviderSimDevice::OnValueChangedStatic, true);

  m_simValueChangedCbKeys[key] = cbKey;
}

void HALSimWSProviderSimDevice::OnValueChangedStatic(
    const char* name, void* param, HAL_SimValueHandle handle, int32_t direction,
    const struct HAL_Value* value) {
  auto valueData = (reinterpret_cast<SimDeviceValueData*>(param));
  valueData->device->OnValueChanged(valueData, value);
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
        ProcessHalCallback(
            {{valueData->key, value->data.v_double + valueData->doubleOffset}});
        break;
      case HAL_ENUM: {
        int v = value->data.v_enum;
        if (v >= 0 && v < static_cast<int>(valueData->optionValues.size())) {
          ProcessHalCallback({{valueData->key, valueData->optionValues[v]}});
        } else if (v >= 0 && v < static_cast<int>(valueData->options.size())) {
          ProcessHalCallback({{valueData->key, valueData->options[v]}});
        }
        break;
      }
      case HAL_INT:
        ProcessHalCallback(
            {{valueData->key, value->data.v_int + valueData->intOffset}});
        break;
      case HAL_LONG:
        ProcessHalCallback(
            {{valueData->key, value->data.v_long + valueData->intOffset}});
        break;
      default:
        break;
    }
  }
}

void HALSimWSProviderSimDevice::OnValueResetStatic(
    const char* name, void* param, HAL_SimValueHandle handle, int32_t direction,
    const struct HAL_Value* value) {
  auto valueData = (reinterpret_cast<SimDeviceValueData*>(param));
  valueData->device->OnValueReset(valueData, value);
}

void HALSimWSProviderSimDevice::OnValueReset(SimDeviceValueData* valueData,
                                             const struct HAL_Value* value) {
  switch (value->type) {
    case HAL_BOOLEAN:
    case HAL_ENUM:
      break;
    case HAL_DOUBLE:
      valueData->doubleOffset += value->data.v_double;
      break;
    case HAL_INT:
      valueData->intOffset += value->data.v_int;
      break;
    case HAL_LONG:
      valueData->intOffset += value->data.v_long;
      break;
    default:
      break;
  }
}

void HALSimWSProviderSimDevice::ProcessHalCallback(const wpi::json& payload) {
  auto ws = m_ws.lock();
  if (ws) {
    wpi::json netValue = {
        {"type", m_type}, {"device", m_deviceId}, {"data", payload}};
    ws->OnSimValueChanged(netValue);
  }
}

HALSimWSProviderSimDevices::~HALSimWSProviderSimDevices() {
  CancelCallbacks();
}

void HALSimWSProviderSimDevices::DeviceCreatedCallback(
    const char* name, HAL_SimDeviceHandle handle) {
  // Map "Accel:Foo" -> type=Accel, device=Foo
  auto [type, id] = wpi::StringRef{name}.split(':');
  std::shared_ptr<HALSimWSProviderSimDevice> dev;
  if (id.empty()) {
    auto key = ("SimDevice/" + type).str();
    dev = std::make_shared<HALSimWSProviderSimDevice>(handle, key, "SimDevice",
                                                      type);
    m_providers.Add(key, dev);
  } else {
    auto key = (type + "/" + id).str();
    dev = std::make_shared<HALSimWSProviderSimDevice>(handle, key, type, id);
    m_providers.Add(key, dev);
  }

  if (m_ws) {
    m_exec->Call([this, dev]() { dev->OnNetworkConnected(GetWSConnection()); });
  }
}

void HALSimWSProviderSimDevices::DeviceFreedCallback(
    const char* name, HAL_SimDeviceHandle handle) {
  m_providers.Delete(name);
}

void HALSimWSProviderSimDevices::Initialize(wpi::uv::Loop& loop) {
  m_deviceCreatedCbKey = HALSIM_RegisterSimDeviceCreatedCallback(
      "", this, HALSimWSProviderSimDevices::DeviceCreatedCallbackStatic, 1);
  m_deviceFreedCbKey = HALSIM_RegisterSimDeviceFreedCallback(
      "", this, HALSimWSProviderSimDevices::DeviceFreedCallbackStatic, false);

  m_exec = UvExecFn::Create(loop, [](auto out, LoopFn func) {
    func();
    out.set_value();
  });
}

void HALSimWSProviderSimDevices::CancelCallbacks() {
  HALSIM_CancelSimDeviceCreatedCallback(m_deviceCreatedCbKey);
  HALSIM_CancelSimDeviceFreedCallback(m_deviceFreedCbKey);

  m_deviceCreatedCbKey = 0;
  m_deviceFreedCbKey = 0;
}

void HALSimWSProviderSimDevices::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  m_ws = hws;
}

void HALSimWSProviderSimDevices::OnNetworkDisconnected() {
  m_ws = nullptr;
}

}  // namespace wpilibws
