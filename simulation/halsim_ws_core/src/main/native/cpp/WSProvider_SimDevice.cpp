// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/hal/Ports.h"
#include "wpi/util/StringExtras.hpp"

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
    HALSIM_CancelSimValueChangedCallback(kv.second);
  }

  m_simValueChangedCbKeys.clear();
}

void HALSimWSProviderSimDevice::OnNetValueChanged(const wpi::util::json& json) {
  std::shared_lock lock(m_vhLock);
  for (auto&& [key, jvalue] : json.get_object()) {
    auto vd = m_valueHandles.find(key);
    if (vd != m_valueHandles.end()) {
      HAL_Value value;
      value.type = vd->second.valueType;
      switch (value.type) {
        case HAL_BOOLEAN:
          if (jvalue.is_bool()) {
            value.data.v_boolean = jvalue.get_bool() ? 1 : 0;
          }
          break;
        case HAL_DOUBLE:
          if (jvalue.is_number()) {
            value.data.v_double = jvalue.get_double();
            value.data.v_double -= vd->second.doubleOffset;
          }
          break;
        case HAL_ENUM: {
          if (jvalue.is_string()) {
            auto& options = vd->second.options;
            auto& str = jvalue.get_string();
            auto optionIt =
                std::find_if(options.begin(), options.end(),
                             [&](const std::string& v) { return v == str; });
            if (optionIt != options.end()) {
              value.data.v_enum = optionIt - options.begin();
            }
          } else if (jvalue.is_float() || jvalue.is_double()) {
            auto& values = vd->second.optionValues;
            double num = jvalue.get_number();
            auto valueIt = std::find_if(
                values.begin(), values.end(),
                [&](double v) { return std::fabs(v - num) < 1e-4; });
            if (valueIt != values.end()) {
              value.data.v_enum = valueIt - values.begin();
            }
          } else if (jvalue.is_int()) {
            value.data.v_enum = jvalue.get_int();
          }
          break;
        }
        case HAL_INT:
          if (jvalue.is_int()) {
            value.data.v_int = jvalue.get_int();
            value.data.v_int -= vd->second.intOffset;
          }
          break;
        case HAL_LONG:
          if (jvalue.is_int()) {
            value.data.v_long = jvalue.get_int();
            value.data.v_long -= vd->second.intOffset;
          }
          break;
        default:
          break;
      }

      HAL_SetSimValue(vd->second.handle, &value);
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
      case HAL_SIM_VALUE_INPUT:
        prefix = ">";
        break;
      case HAL_SIM_VALUE_OUTPUT:
        prefix = "<";
        break;
      case HAL_SIM_VALUE_BIDIR:
        prefix = "<>";
        break;
      default:
        break;
    }
  }
  std::string key = fmt::format("{}{}", prefix, name);
  SimDeviceValueData data;
  data.device = this;
  data.handle = handle;
  data.key = key;
  if (value->type == HAL_ENUM) {
    int32_t numOptions = 0;

    const char** options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);
    data.options.reserve(numOptions);
    for (int32_t i = 0; i < numOptions; ++i) {
      data.options.emplace_back(options[i]);
    }

    const double* values =
        HALSIM_GetSimValueEnumDoubleValues(handle, &numOptions);
    data.optionValues.assign(values, values + numOptions);
  }
  data.valueType = value->type;

  SimDeviceValueData* param;

  {
    std::unique_lock lock(m_vhLock);
    param = &m_valueHandles.insert_or_assign(data.key, std::move(data))
                 .first->second;
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
        ProcessHalCallback(wpi::util::json::object(
            valueData->key, static_cast<bool>(value->data.v_boolean)));
        break;
      case HAL_DOUBLE:
        ProcessHalCallback(wpi::util::json::object(
            valueData->key, value->data.v_double + valueData->doubleOffset));
        break;
      case HAL_ENUM: {
        int v = value->data.v_enum;
        if (v >= 0 && v < static_cast<int>(valueData->optionValues.size())) {
          ProcessHalCallback(wpi::util::json::object(
              valueData->key, valueData->optionValues[v]));
        } else if (v >= 0 && v < static_cast<int>(valueData->options.size())) {
          ProcessHalCallback(
              wpi::util::json::object(valueData->key, valueData->options[v]));
        }
        break;
      }
      case HAL_INT:
        ProcessHalCallback(wpi::util::json::object(
            valueData->key, value->data.v_int + valueData->intOffset));
        break;
      case HAL_LONG:
        ProcessHalCallback(wpi::util::json::object(
            valueData->key, value->data.v_long + valueData->intOffset));
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

void HALSimWSProviderSimDevice::ProcessHalCallback(
    const wpi::util::json& payload) {
  auto ws = m_ws.lock();
  if (ws) {
    auto netValue = wpi::util::json::object();
    netValue["type"] = m_type;
    netValue["device"] = m_deviceId;
    netValue["data"] = payload;
    ws->OnSimValueChanged(netValue);
  }
}

HALSimWSProviderSimDevices::~HALSimWSProviderSimDevices() {
  CancelCallbacks();
}

void HALSimWSProviderSimDevices::DeviceCreatedCallback(
    const char* name, HAL_SimDeviceHandle handle) {
  // Map "Accel:Foo" -> type=Accel, device=Foo
  auto [type, id] = wpi::util::split(name, ':');
  std::shared_ptr<HALSimWSProviderSimDevice> dev;
  if (id.empty()) {
    auto key = fmt::format("SimDevice/{}", type);
    dev = std::make_shared<HALSimWSProviderSimDevice>(handle, key, "SimDevice",
                                                      type);
    m_providers.Add(key, dev);
  } else {
    auto key = fmt::format("{}/{}", type, id);
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

void HALSimWSProviderSimDevices::Initialize(wpi::net::uv::Loop& loop) {
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
