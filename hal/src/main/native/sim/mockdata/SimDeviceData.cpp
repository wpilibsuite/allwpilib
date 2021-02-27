// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/SimDeviceData.h"  // NOLINT(build/include_order)

#include <algorithm>

#include "SimDeviceDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeSimDeviceData() {
  static SimDeviceData sdd;
  ::hal::SimSimDeviceData = &sdd;
}
}  // namespace hal::init

SimDeviceData* hal::SimSimDeviceData;

SimDeviceData::Device* SimDeviceData::LookupDevice(HAL_SimDeviceHandle handle) {
  if (handle <= 0) {
    return nullptr;
  }
  --handle;
  if (static_cast<uint32_t>(handle) >= m_devices.size() || !m_devices[handle]) {
    return nullptr;
  }
  return m_devices[handle].get();
}

SimDeviceData::Value* SimDeviceData::LookupValue(HAL_SimValueHandle handle) {
  if (handle <= 0) {
    return nullptr;
  }

  // look up device
  Device* deviceImpl = LookupDevice(handle >> 16);
  if (!deviceImpl) {
    return nullptr;
  }

  // look up value
  handle &= 0xffff;
  --handle;
  if (static_cast<uint32_t>(handle) >= deviceImpl->values.size() ||
      !deviceImpl->values[handle]) {
    return nullptr;
  }

  return deviceImpl->values[handle].get();
}

void SimDeviceData::SetDeviceEnabled(const char* prefix, bool enabled) {
  std::scoped_lock lock(m_mutex);
  auto it =
      std::find_if(m_prefixEnabled.begin(), m_prefixEnabled.end(),
                   [=](const auto& elem) { return elem.first == prefix; });
  if (it != m_prefixEnabled.end()) {
    it->second = enabled;
    return;
  }
  m_prefixEnabled.emplace_back(prefix, enabled);
  // keep it sorted by name
  // string comparison sorts shorter before longer, so reverse the sort
  std::sort(m_prefixEnabled.begin(), m_prefixEnabled.end(),
            [](const auto& l, const auto& r) { return l.first >= r.first; });
}

bool SimDeviceData::IsDeviceEnabled(const char* name) {
  std::scoped_lock lock(m_mutex);
  for (const auto& elem : m_prefixEnabled) {
    if (wpi::StringRef{name}.startswith(elem.first)) {
      return elem.second;
    }
  }
  return true;
}

HAL_SimDeviceHandle SimDeviceData::CreateDevice(const char* name) {
  std::scoped_lock lock(m_mutex);

  // don't create if disabled
  for (const auto& elem : m_prefixEnabled) {
    if (wpi::StringRef{name}.startswith(elem.first)) {
      if (elem.second) {
        break;  // enabled
      }
      return 0;  // disabled
    }
  }

  // check for duplicates and don't overwrite them
  if (m_deviceMap.count(name) > 0) {
    return 0;
  }

  // don't allow more than 4096 devices (limit driven by 12-bit allocation in
  // value changed callback uid)
  if (m_devices.size() >= 4095) {
    return 0;
  }

  // create and save
  auto deviceImpl = std::make_shared<Device>(name);
  HAL_SimDeviceHandle deviceHandle = m_devices.emplace_back(deviceImpl) + 1;
  deviceImpl->handle = deviceHandle;
  m_deviceMap[name] = deviceImpl;

  // notify callbacks
  m_deviceCreated(name, deviceHandle);

  return deviceHandle;
}

void SimDeviceData::FreeDevice(HAL_SimDeviceHandle handle) {
  std::scoped_lock lock(m_mutex);
  --handle;

  // see if it exists
  if (handle < 0 || static_cast<uint32_t>(handle) >= m_devices.size()) {
    return;
  }
  auto deviceImpl = std::move(m_devices[handle]);
  if (!deviceImpl) {
    return;
  }

  // remove from map
  m_deviceMap.erase(deviceImpl->name);

  // remove from vector
  m_devices.erase(handle);

  // notify callbacks
  m_deviceFreed(deviceImpl->name.c_str(), handle + 1);
}

HAL_SimValueHandle SimDeviceData::CreateValue(
    HAL_SimDeviceHandle device, const char* name, int32_t direction,
    int32_t numOptions, const char** options, const double* optionValues,
    const HAL_Value& initialValue) {
  std::scoped_lock lock(m_mutex);

  // look up device
  Device* deviceImpl = LookupDevice(device);
  if (!deviceImpl) {
    return 0;
  }

  // check for duplicates and don't overwrite them
  auto it = deviceImpl->valueMap.find(name);
  if (it != deviceImpl->valueMap.end()) {
    return 0;
  }

  // don't allow more than 4096 values per device (limit driven by 12-bit
  // allocation in value changed callback uid)
  if (deviceImpl->values.size() >= 4095) {
    return 0;
  }

  // create and save; encode device into handle
  auto valueImplPtr = std::make_unique<Value>(name, direction, initialValue);
  Value* valueImpl = valueImplPtr.get();
  HAL_SimValueHandle valueHandle =
      (device << 16) |
      (deviceImpl->values.emplace_back(std::move(valueImplPtr)) + 1);
  valueImpl->handle = valueHandle;
  // copy options (if any provided)
  if (numOptions > 0 && options) {
    valueImpl->enumOptions.reserve(numOptions);
    valueImpl->cstrEnumOptions.reserve(numOptions);
    for (int32_t i = 0; i < numOptions; ++i) {
      valueImpl->enumOptions.emplace_back(options[i]);
      // point to our copy of the string, not the passed-in one
      valueImpl->cstrEnumOptions.emplace_back(
          valueImpl->enumOptions.back().c_str());
    }
  }
  // copy option values (if any provided)
  if (numOptions > 0 && optionValues) {
    valueImpl->enumOptionValues.assign(optionValues, optionValues + numOptions);
  }
  deviceImpl->valueMap[name] = valueImpl;

  // notify callbacks
  deviceImpl->valueCreated(name, valueHandle, direction, &initialValue);

  return valueHandle;
}

HAL_Value SimDeviceData::GetValue(HAL_SimValueHandle handle) {
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);

  if (!valueImpl) {
    HAL_Value value;
    value.data.v_int = 0;
    value.type = HAL_UNASSIGNED;
    return value;
  }

  return valueImpl->value;
}

void SimDeviceData::SetValue(HAL_SimValueHandle handle,
                             const HAL_Value& value) {
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return;
  }

  valueImpl->value = value;

  // notify callbacks
  valueImpl->changed(valueImpl->name.c_str(), valueImpl->handle,
                     valueImpl->direction, &value);
}

void SimDeviceData::ResetValue(HAL_SimValueHandle handle) {
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return;
  }

  // don't notify reset if we aren't going to actually reset anything
  switch (valueImpl->value.type) {
    case HAL_INT:
    case HAL_LONG:
    case HAL_DOUBLE:
      break;
    default:
      return;
  }

  // notify reset callbacks (done here so they're called with the old value)
  valueImpl->reset(valueImpl->name.c_str(), valueImpl->handle,
                   valueImpl->direction, &valueImpl->value);

  // set user-facing value to 0
  switch (valueImpl->value.type) {
    case HAL_INT:
      valueImpl->value.data.v_int = 0;
      break;
    case HAL_LONG:
      valueImpl->value.data.v_long = 0;
      break;
    case HAL_DOUBLE:
      valueImpl->value.data.v_double = 0;
      break;
    default:
      return;
  }

  // notify changed callbacks
  valueImpl->changed(valueImpl->name.c_str(), valueImpl->handle,
                     valueImpl->direction, &valueImpl->value);
}

int32_t SimDeviceData::RegisterDeviceCreatedCallback(
    const char* prefix, void* param, HALSIM_SimDeviceCallback callback,
    bool initialNotify) {
  std::scoped_lock lock(m_mutex);

  // register callback
  int32_t index = m_deviceCreated.Register(prefix, param, callback);

  // initial notifications
  if (initialNotify) {
    for (auto&& device : m_devices) {
      if (wpi::StringRef{device->name}.startswith(prefix)) {
        callback(device->name.c_str(), param, device->handle);
      }
    }
  }

  return index;
}

void SimDeviceData::CancelDeviceCreatedCallback(int32_t uid) {
  if (uid <= 0) {
    return;
  }
  std::scoped_lock lock(m_mutex);
  m_deviceCreated.Cancel(uid);
}

int32_t SimDeviceData::RegisterDeviceFreedCallback(
    const char* prefix, void* param, HALSIM_SimDeviceCallback callback) {
  std::scoped_lock lock(m_mutex);
  return m_deviceFreed.Register(prefix, param, callback);
}

void SimDeviceData::CancelDeviceFreedCallback(int32_t uid) {
  if (uid <= 0) {
    return;
  }
  std::scoped_lock lock(m_mutex);
  m_deviceFreed.Cancel(uid);
}

HAL_SimDeviceHandle SimDeviceData::GetDeviceHandle(const char* name) {
  std::scoped_lock lock(m_mutex);
  auto it = m_deviceMap.find(name);
  if (it == m_deviceMap.end()) {
    return 0;
  }
  if (auto deviceImpl = it->getValue().lock()) {
    return deviceImpl->handle;
  } else {
    return 0;
  }
}

const char* SimDeviceData::GetDeviceName(HAL_SimDeviceHandle handle) {
  std::scoped_lock lock(m_mutex);

  // look up device
  Device* deviceImpl = LookupDevice(handle);
  if (!deviceImpl) {
    return nullptr;
  }

  return deviceImpl->name.c_str();
}

void SimDeviceData::EnumerateDevices(const char* prefix, void* param,
                                     HALSIM_SimDeviceCallback callback) {
  std::scoped_lock lock(m_mutex);
  for (auto&& device : m_devices) {
    if (wpi::StringRef{device->name}.startswith(prefix)) {
      callback(device->name.c_str(), param, device->handle);
    }
  }
}

int32_t SimDeviceData::RegisterValueCreatedCallback(
    HAL_SimDeviceHandle device, void* param, HALSIM_SimValueCallback callback,
    bool initialNotify) {
  std::scoped_lock lock(m_mutex);
  Device* deviceImpl = LookupDevice(device);
  if (!deviceImpl) {
    return -1;
  }

  // register callback
  int32_t index = deviceImpl->valueCreated.Register(callback, param);

  // initial notifications
  if (initialNotify) {
    for (auto&& value : deviceImpl->values) {
      callback(value->name.c_str(), param, value->handle, value->direction,
               &value->value);
    }
  }

  // encode device into uid
  return (device << 16) | (index & 0xffff);
}

void SimDeviceData::CancelValueCreatedCallback(int32_t uid) {
  if (uid <= 0) {
    return;
  }
  std::scoped_lock lock(m_mutex);
  Device* deviceImpl = LookupDevice(uid >> 16);
  if (!deviceImpl) {
    return;
  }
  deviceImpl->valueCreated.Cancel(uid & 0xffff);
}

int32_t SimDeviceData::RegisterValueChangedCallback(
    HAL_SimValueHandle handle, void* param, HALSIM_SimValueCallback callback,
    bool initialNotify) {
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return -1;
  }

  // register callback
  int32_t index = valueImpl->changed.Register(callback, param);

  // initial notification
  if (initialNotify) {
    callback(valueImpl->name.c_str(), param, valueImpl->handle,
             valueImpl->direction, &valueImpl->value);
  }

  // encode device and value into uid
  return (((handle >> 16) & 0xfff) << 19) | ((handle & 0xfff) << 7) |
         (index & 0x7f);
}

void SimDeviceData::CancelValueChangedCallback(int32_t uid) {
  if (uid <= 0) {
    return;
  }
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(((uid >> 19) << 16) | ((uid >> 7) & 0xfff));
  if (!valueImpl) {
    return;
  }
  valueImpl->changed.Cancel(uid & 0x7f);
}

int32_t SimDeviceData::RegisterValueResetCallback(
    HAL_SimValueHandle handle, void* param, HALSIM_SimValueCallback callback,
    bool initialNotify) {
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return -1;
  }

  // register callback
  int32_t index = valueImpl->reset.Register(callback, param);

  // encode device and value into uid
  return (((handle >> 16) & 0xfff) << 19) | ((handle & 0xfff) << 7) |
         (index & 0x7f);
}

void SimDeviceData::CancelValueResetCallback(int32_t uid) {
  if (uid <= 0) {
    return;
  }
  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(((uid >> 19) << 16) | ((uid >> 7) & 0xfff));
  if (!valueImpl) {
    return;
  }
  valueImpl->reset.Cancel(uid & 0x7f);
}

HAL_SimValueHandle SimDeviceData::GetValueHandle(HAL_SimDeviceHandle device,
                                                 const char* name) {
  std::scoped_lock lock(m_mutex);
  Device* deviceImpl = LookupDevice(device);
  if (!deviceImpl) {
    return 0;
  }

  // lookup value
  auto it = deviceImpl->valueMap.find(name);
  if (it == deviceImpl->valueMap.end()) {
    return 0;
  }
  if (!it->getValue()) {
    return 0;
  }
  return it->getValue()->handle;
}

void SimDeviceData::EnumerateValues(HAL_SimDeviceHandle device, void* param,
                                    HALSIM_SimValueCallback callback) {
  std::scoped_lock lock(m_mutex);
  Device* deviceImpl = LookupDevice(device);
  if (!deviceImpl) {
    return;
  }

  for (auto&& value : deviceImpl->values) {
    callback(value->name.c_str(), param, value->handle, value->direction,
             &value->value);
  }
}

const char** SimDeviceData::GetValueEnumOptions(HAL_SimValueHandle handle,
                                                int32_t* numOptions) {
  *numOptions = 0;

  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return nullptr;
  }

  // get list of options (safe to return as they never change)
  auto& options = valueImpl->cstrEnumOptions;
  *numOptions = options.size();
  return options.data();
}

const double* SimDeviceData::GetValueEnumDoubleValues(HAL_SimValueHandle handle,
                                                      int32_t* numOptions) {
  *numOptions = 0;

  std::scoped_lock lock(m_mutex);
  Value* valueImpl = LookupValue(handle);
  if (!valueImpl) {
    return nullptr;
  }

  // get list of option values (safe to return as they never change)
  auto& optionValues = valueImpl->enumOptionValues;
  *numOptions = optionValues.size();
  return optionValues.data();
}

void SimDeviceData::ResetData() {
  std::scoped_lock lock(m_mutex);
  m_devices.clear();
  m_deviceMap.clear();
  m_prefixEnabled.clear();
  m_deviceCreated.Reset();
  m_deviceFreed.Reset();
}

extern "C" {

void HALSIM_SetSimDeviceEnabled(const char* prefix, HAL_Bool enabled) {
  SimSimDeviceData->SetDeviceEnabled(prefix, enabled);
}

HAL_Bool HALSIM_IsSimDeviceEnabled(const char* name) {
  return SimSimDeviceData->IsDeviceEnabled(name);
}

int32_t HALSIM_RegisterSimDeviceCreatedCallback(
    const char* prefix, void* param, HALSIM_SimDeviceCallback callback,
    HAL_Bool initialNotify) {
  return SimSimDeviceData->RegisterDeviceCreatedCallback(
      prefix, param, callback, initialNotify);
}

void HALSIM_CancelSimDeviceCreatedCallback(int32_t uid) {
  SimSimDeviceData->CancelDeviceCreatedCallback(uid);
}

int32_t HALSIM_RegisterSimDeviceFreedCallback(const char* prefix, void* param,
                                              HALSIM_SimDeviceCallback callback,
                                              HAL_Bool initialNotify) {
  return SimSimDeviceData->RegisterDeviceFreedCallback(prefix, param, callback);
}

void HALSIM_CancelSimDeviceFreedCallback(int32_t uid) {
  SimSimDeviceData->CancelDeviceFreedCallback(uid);
}

HAL_SimDeviceHandle HALSIM_GetSimDeviceHandle(const char* name) {
  return SimSimDeviceData->GetDeviceHandle(name);
}

const char* HALSIM_GetSimDeviceName(HAL_SimDeviceHandle handle) {
  return SimSimDeviceData->GetDeviceName(handle);
}

HAL_SimDeviceHandle HALSIM_GetSimValueDeviceHandle(HAL_SimValueHandle handle) {
  if (handle <= 0) {
    return 0;
  }
  return handle >> 16;
}

void HALSIM_EnumerateSimDevices(const char* prefix, void* param,
                                HALSIM_SimDeviceCallback callback) {
  SimSimDeviceData->EnumerateDevices(prefix, param, callback);
}

int32_t HALSIM_RegisterSimValueCreatedCallback(HAL_SimDeviceHandle device,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify) {
  return SimSimDeviceData->RegisterValueCreatedCallback(device, param, callback,
                                                        initialNotify);
}

void HALSIM_CancelSimValueCreatedCallback(int32_t uid) {
  SimSimDeviceData->CancelValueCreatedCallback(uid);
}

int32_t HALSIM_RegisterSimValueChangedCallback(HAL_SimValueHandle handle,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify) {
  return SimSimDeviceData->RegisterValueChangedCallback(handle, param, callback,
                                                        initialNotify);
}

void HALSIM_CancelSimValueChangedCallback(int32_t uid) {
  SimSimDeviceData->CancelValueChangedCallback(uid);
}

int32_t HALSIM_RegisterSimValueResetCallback(HAL_SimValueHandle handle,
                                             void* param,
                                             HALSIM_SimValueCallback callback,
                                             HAL_Bool initialNotify) {
  return SimSimDeviceData->RegisterValueResetCallback(handle, param, callback,
                                                      initialNotify);
}

void HALSIM_CancelSimValueResetCallback(int32_t uid) {
  SimSimDeviceData->CancelValueResetCallback(uid);
}

HAL_SimValueHandle HALSIM_GetSimValueHandle(HAL_SimDeviceHandle device,
                                            const char* name) {
  return SimSimDeviceData->GetValueHandle(device, name);
}

void HALSIM_EnumerateSimValues(HAL_SimDeviceHandle device, void* param,
                               HALSIM_SimValueCallback callback) {
  SimSimDeviceData->EnumerateValues(device, param, callback);
}

const char** HALSIM_GetSimValueEnumOptions(HAL_SimValueHandle handle,
                                           int32_t* numOptions) {
  return SimSimDeviceData->GetValueEnumOptions(handle, numOptions);
}

const double* HALSIM_GetSimValueEnumDoubleValues(HAL_SimValueHandle handle,
                                                 int32_t* numOptions) {
  return SimSimDeviceData->GetValueEnumDoubleValues(handle, numOptions);
}

void HALSIM_ResetSimDeviceData(void) {
  SimSimDeviceData->ResetData();
}

}  // extern "C"
