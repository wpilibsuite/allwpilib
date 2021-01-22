// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <wpi/Compiler.h>
#include <wpi/UidVector.h>
#include <wpi/spinlock.h>

#include "hal/simulation/NotifyListener.h"

namespace hal {

namespace impl {

class SimCallbackRegistryBase {
 public:
  using RawFunctor = void (*)();

 protected:
  using CallbackVector = wpi::UidVector<HalCallbackListener<RawFunctor>, 4>;

 public:
  void Cancel(int32_t uid) {
    std::scoped_lock lock(m_mutex);
    if (m_callbacks && uid > 0) {
      m_callbacks->erase(uid - 1);
    }
  }

  void Reset() {
    std::scoped_lock lock(m_mutex);
    DoReset();
  }

  wpi::recursive_spinlock& GetMutex() { return m_mutex; }

 protected:
  int32_t DoRegister(RawFunctor callback, void* param) {
    // Must return -1 on a null callback for error handling
    if (callback == nullptr) {
      return -1;
    }
    if (!m_callbacks) {
      m_callbacks = std::make_unique<CallbackVector>();
    }
    return m_callbacks->emplace_back(param, callback) + 1;
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE void DoReset() {
    if (m_callbacks) {
      m_callbacks->clear();
    }
  }

  mutable wpi::recursive_spinlock m_mutex;
  std::unique_ptr<CallbackVector> m_callbacks;
};

}  // namespace impl

/**
 * Simulation callback registry.  Provides callback functionality.
 *
 * @tparam CallbackFunction callback function type (e.g. HAL_BufferCallback)
 * @tparam GetName function that returns a const char* for the name
 */
template <typename CallbackFunction, const char* (*GetName)()>
class SimCallbackRegistry : public impl::SimCallbackRegistryBase {
 public:
  int32_t Register(CallbackFunction callback, void* param) {
    std::scoped_lock lock(m_mutex);
    return DoRegister(reinterpret_cast<RawFunctor>(callback), param);
  }

  template <typename... U>
  void Invoke(U&&... u) const {
#ifdef _MSC_VER  // work around VS2019 16.4.0 bug
    std::scoped_lock<wpi::recursive_spinlock> lock(m_mutex);
#else
    std::scoped_lock lock(m_mutex);
#endif
    if (m_callbacks) {
      const char* name = GetName();
      for (auto&& cb : *m_callbacks) {
        reinterpret_cast<CallbackFunction>(cb.callback)(name, cb.param,
                                                        std::forward<U>(u)...);
      }
    }
  }

  template <typename... U>
  LLVM_ATTRIBUTE_ALWAYS_INLINE void operator()(U&&... u) const {
    return Invoke(std::forward<U>(u)...);
  }
};

/**
 * Define a name functor for use with SimCallbackRegistry.
 * This creates a function named GetNAMEName() that returns "NAME".
 * @param NAME the name to return
 */
#define HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(NAME)           \
  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr const char* \
      Get##NAME##Name() {                                   \
    return #NAME;                                           \
  }

/**
 * Define a standard C API for SimCallbackRegistry.
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, TYPE callback, void* param)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t uid)
 *
 * @param TYPE the underlying callback type (e.g. HAL_BufferCallback)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param DATA the backing data array
 * @param LOWERNAME the lowercase name of the backing data registry
 */
#define HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI(TYPE, NS, CAPINAME, DATA,     \
                                            LOWERNAME)                    \
  int32_t NS##_Register##CAPINAME##Callback(int32_t index, TYPE callback, \
                                            void* param) {                \
    return DATA[index].LOWERNAME.Register(callback, param);               \
  }                                                                       \
                                                                          \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t uid) {      \
    DATA[index].LOWERNAME.Cancel(uid);                                    \
  }

/**
 * Define a standard C API for SimCallbackRegistry (no index variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(TYPE callback, void* param)
 * - void NS_CancelCAPINAMECallback(int32_t uid)
 *
 * @param TYPE the underlying callback type (e.g. HAL_BufferCallback)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param DATA the backing data pointer
 * @param LOWERNAME the lowercase name of the backing data registry
 */
#define HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI_NOINDEX(TYPE, NS, CAPINAME, DATA, \
                                                    LOWERNAME)                \
  int32_t NS##_Register##CAPINAME##Callback(TYPE callback, void* param) {     \
    return DATA->LOWERNAME.Register(callback, param);                         \
  }                                                                           \
                                                                              \
  void NS##_Cancel##CAPINAME##Callback(int32_t uid) {                         \
    DATA->LOWERNAME.Cancel(uid);                                              \
  }

/**
 * Define a stub standard C API for SimCallbackRegistry.
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, TYPE callback, void* param)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t uid)
 *
 * @param TYPE the underlying callback type (e.g. HAL_BufferCallback)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 */
#define HAL_SIMCALLBACKREGISTRY_STUB_CAPI(TYPE, NS, CAPINAME)             \
  int32_t NS##_Register##CAPINAME##Callback(int32_t index, TYPE callback, \
                                            void* param) {                \
    return 0;                                                             \
  }                                                                       \
                                                                          \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t uid) {}

/**
 * Define a stub standard C API for SimCallbackRegistry (no index variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(TYPE callback, void* param)
 * - void NS_CancelCAPINAMECallback(int32_t uid)
 *
 * @param TYPE the underlying callback type (e.g. HAL_BufferCallback)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 */
#define HAL_SIMCALLBACKREGISTRY_STUB_CAPI_NOINDEX(TYPE, NS, CAPINAME)     \
  int32_t NS##_Register##CAPINAME##Callback(TYPE callback, void* param) { \
    return 0;                                                             \
  }                                                                       \
                                                                          \
  void NS##_Cancel##CAPINAME##Callback(int32_t uid) {}

}  // namespace hal
