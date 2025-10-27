// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/util/StringExtras.hpp>
#include <wpi/util/StringMap.hpp>
#include <wpi/util/UidVector.hpp>
#include <wpi/util/spinlock.hpp>

#include "wpi/hal/Value.h"
#include "wpi/hal/simulation/SimCallbackRegistry.h"
#include "wpi/hal/simulation/SimDeviceData.h"

namespace wpi::hal {

namespace impl {

template <typename CallbackFunction>
class SimUnnamedCallbackRegistry {
 public:
  using RawFunctor = void (*)();

 protected:
  using CallbackVector = wpi::util::UidVector<HalCallbackListener<RawFunctor>, 4>;

 public:
  void Cancel(int32_t uid) {
    if (m_callbacks) {
      m_callbacks->erase(uid - 1);
    }
  }

  void Reset() {
    if (m_callbacks) {
      m_callbacks->clear();
    }
  }

  int32_t Register(CallbackFunction callback, void* param) {
    // Must return -1 on a null callback for error handling
    if (callback == nullptr) {
      return -1;
    }
    if (!m_callbacks) {
      m_callbacks = std::make_unique<CallbackVector>();
    }
    return m_callbacks->emplace_back(param,
                                     reinterpret_cast<RawFunctor>(callback)) +
           1;
  }

  template <typename Mutex, typename... U>
  void Invoke(std::unique_lock<Mutex>& lock, const char* name, U&&... u) const {
    if (m_callbacks) {
      for (size_t i = 0; i < m_callbacks->size(); ++i) {
        auto& cb = (*m_callbacks)[i];
        if (cb.callback) {
          auto callback = cb.callback;
          auto param = cb.param;
          lock.unlock();
          reinterpret_cast<CallbackFunction>(callback)(name, param,
                                                       std::forward<U>(u)...);
          lock.lock();
        }
      }
    }
  }

  template <typename... U>
  LLVM_ATTRIBUTE_ALWAYS_INLINE void operator()(U&&... u) const {
    return Invoke(std::forward<U>(u)...);
  }

 private:
  std::unique_ptr<CallbackVector> m_callbacks;
};

template <typename CallbackFunction>
class SimPrefixCallbackRegistry {
  struct CallbackData {
    CallbackData() = default;
    CallbackData(const char* prefix_, void* param_, CallbackFunction callback_)
        : prefix{prefix_}, callback{callback_}, param{param_} {}
    std::string prefix;
    CallbackFunction callback;
    void* param;

    explicit operator bool() const { return callback != nullptr; }
  };
  using CallbackVector = wpi::util::UidVector<CallbackData, 4>;

 public:
  void Cancel(int32_t uid) {
    if (m_callbacks) {
      m_callbacks->erase(uid - 1);
    }
  }

  void Reset() {
    if (m_callbacks) {
      m_callbacks->clear();
    }
  }

  int32_t Register(const char* prefix, void* param, CallbackFunction callback) {
    // Must return -1 on a null callback for error handling
    if (callback == nullptr) {
      return -1;
    }
    if (!m_callbacks) {
      m_callbacks = std::make_unique<CallbackVector>();
    }
    return m_callbacks->emplace_back(prefix, param, callback) + 1;
  }

  template <typename Mutex, typename... U>
  void Invoke(std::unique_lock<Mutex>& lock, const char* name, U&&... u) const {
    if (m_callbacks) {
      for (size_t i = 0; i < m_callbacks->size(); ++i) {
        auto& cb = (*m_callbacks)[i];
        if (cb.callback && wpi::util::starts_with(name, cb.prefix)) {
          auto callback = cb.callback;
          auto param = cb.param;
          lock.unlock();
          callback(name, param, std::forward<U>(u)...);
          lock.lock();
        }
      }
    }
  }

  template <typename... U>
  LLVM_ATTRIBUTE_ALWAYS_INLINE void operator()(U&&... u) const {
    return Invoke(std::forward<U>(u)...);
  }

 private:
  std::unique_ptr<CallbackVector> m_callbacks;
};

}  // namespace impl

class SimDeviceData {
 private:
  struct Value {
    Value(const char* name_, int32_t direction_, const HAL_Value& value_)
        : name{name_}, direction{direction_}, value{value_} {}

    HAL_SimValueHandle handle{0};
    std::string name;
    int32_t direction;
    HAL_Value value;
    std::vector<std::string> enumOptions;
    std::vector<const char*> cstrEnumOptions;
    std::vector<double> enumOptionValues;
    impl::SimUnnamedCallbackRegistry<HALSIM_SimValueCallback> changed;
    impl::SimUnnamedCallbackRegistry<HALSIM_SimValueCallback> reset;
  };

  struct Device {
    explicit Device(const char* name_) : name{name_} {}

    HAL_SimDeviceHandle handle{0};
    std::string name;
    wpi::util::UidVector<std::unique_ptr<Value>, 16> values;
    wpi::util::StringMap<Value*> valueMap;
    impl::SimUnnamedCallbackRegistry<HALSIM_SimValueCallback> valueCreated;
  };

  wpi::util::UidVector<std::shared_ptr<Device>, 4> m_devices;
  wpi::util::StringMap<std::weak_ptr<Device>> m_deviceMap;
  std::vector<std::pair<std::string, bool>> m_prefixEnabled;

  wpi::util::recursive_spinlock m_mutex;

  impl::SimPrefixCallbackRegistry<HALSIM_SimDeviceCallback> m_deviceCreated;
  impl::SimPrefixCallbackRegistry<HALSIM_SimDeviceCallback> m_deviceFreed;

  // call with lock held, returns null if does not exist
  Device* LookupDevice(HAL_SimDeviceHandle handle);
  Value* LookupValue(HAL_SimValueHandle handle);

 public:
  void SetDeviceEnabled(const char* prefix, bool enabled);
  bool IsDeviceEnabled(const char* name);

  HAL_SimDeviceHandle CreateDevice(const char* name);
  void FreeDevice(HAL_SimDeviceHandle handle);
  HAL_SimValueHandle CreateValue(HAL_SimDeviceHandle device, const char* name,
                                 int32_t direction, int32_t numOptions,
                                 const char** options,
                                 const double* optionValues,
                                 const HAL_Value& initialValue);
  HAL_Value GetValue(HAL_SimValueHandle handle);
  void SetValue(HAL_SimValueHandle handle, const HAL_Value& value);
  void ResetValue(HAL_SimValueHandle handle);

  int32_t RegisterDeviceCreatedCallback(const char* prefix, void* param,
                                        HALSIM_SimDeviceCallback callback,
                                        bool initialNotify);

  void CancelDeviceCreatedCallback(int32_t uid);

  int32_t RegisterDeviceFreedCallback(const char* prefix, void* param,
                                      HALSIM_SimDeviceCallback callback);

  void CancelDeviceFreedCallback(int32_t uid);

  HAL_SimDeviceHandle GetDeviceHandle(const char* name);
  const char* GetDeviceName(HAL_SimDeviceHandle handle);

  void EnumerateDevices(const char* prefix, void* param,
                        HALSIM_SimDeviceCallback callback);

  int32_t RegisterValueCreatedCallback(HAL_SimDeviceHandle device, void* param,
                                       HALSIM_SimValueCallback callback,
                                       bool initialNotify);

  void CancelValueCreatedCallback(int32_t uid);

  int32_t RegisterValueChangedCallback(HAL_SimValueHandle handle, void* param,
                                       HALSIM_SimValueCallback callback,
                                       bool initialNotify);

  void CancelValueChangedCallback(int32_t uid);

  int32_t RegisterValueResetCallback(HAL_SimValueHandle handle, void* param,
                                     HALSIM_SimValueCallback callback,
                                     bool initialNotify);

  void CancelValueResetCallback(int32_t uid);

  HAL_SimValueHandle GetValueHandle(HAL_SimDeviceHandle device,
                                    const char* name);

  void EnumerateValues(HAL_SimDeviceHandle device, void* param,
                       HALSIM_SimValueCallback callback);

  const char** GetValueEnumOptions(HAL_SimValueHandle handle,
                                   int32_t* numOptions);

  const double* GetValueEnumDoubleValues(HAL_SimValueHandle handle,
                                         int32_t* numOptions);

  void ResetData();
};
extern SimDeviceData* SimSimDeviceData;
}  // namespace wpi::hal
