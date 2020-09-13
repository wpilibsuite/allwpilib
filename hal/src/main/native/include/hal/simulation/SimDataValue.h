/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/Compiler.h>
#include <wpi/UidVector.h>
#include <wpi/spinlock.h>

#include "hal/simulation/NotifyListener.h"
#include "hal/simulation/SimCallbackRegistry.h"

namespace hal {

namespace impl {
template <typename T, HAL_Value (*MakeValue)(T)>
class SimDataValueBase : protected SimCallbackRegistryBase {
 public:
  explicit SimDataValueBase(T value) : m_value(value) {}

  LLVM_ATTRIBUTE_ALWAYS_INLINE void CancelCallback(int32_t uid) { Cancel(uid); }

  T Get() const {
    std::scoped_lock lock(m_mutex);
    return m_value;
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE operator T() const { return Get(); }

  void Reset(T value) {
    std::scoped_lock lock(m_mutex);
    DoReset();
    m_value = value;
  }

  wpi::recursive_spinlock& GetMutex() { return m_mutex; }

 protected:
  int32_t DoRegisterCallback(HAL_NotifyCallback callback, void* param,
                             HAL_Bool initialNotify, const char* name) {
    std::unique_lock lock(m_mutex);
    int32_t newUid = DoRegister(reinterpret_cast<RawFunctor>(callback), param);
    if (newUid == -1) return -1;
    if (initialNotify) {
      // We know that the callback is not null because of earlier null check
      HAL_Value value = MakeValue(m_value);
      lock.unlock();
      callback(name, param, &value);
    }
    return newUid;
  }

  void DoSet(T value, const char* name) {
    std::scoped_lock lock(this->m_mutex);
    if (m_value != value) {
      m_value = value;
      if (m_callbacks) {
        HAL_Value halValue = MakeValue(value);
        for (auto&& cb : *m_callbacks)
          reinterpret_cast<HAL_NotifyCallback>(cb.callback)(name, cb.param,
                                                            &halValue);
      }
    }
  }

  T m_value;
};
}  // namespace impl

/**
 * Simulation data value wrapper.  Provides callback functionality when the
 * data value is changed.
 *
 * @tparam T value type (e.g. double)
 * @tparam MakeValue function that takes a T and returns a HAL_Value
 * @tparam GetName function that returns a const char* for the name
 * @tparam GetDefault function that returns the default T (used only for
 *                    default constructor)
 */
template <typename T, HAL_Value (*MakeValue)(T), const char* (*GetName)(),
          T (*GetDefault)() = nullptr>
class SimDataValue final : public impl::SimDataValueBase<T, MakeValue> {
 public:
  SimDataValue()
      : impl::SimDataValueBase<T, MakeValue>(
            GetDefault != nullptr ? GetDefault() : T()) {}
  explicit SimDataValue(T value)
      : impl::SimDataValueBase<T, MakeValue>(value) {}

  LLVM_ATTRIBUTE_ALWAYS_INLINE int32_t RegisterCallback(
      HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
    return this->DoRegisterCallback(callback, param, initialNotify, GetName());
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE void Set(T value) {
    this->DoSet(value, GetName());
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE SimDataValue& operator=(T value) {
    Set(value);
    return *this;
  }
};

/**
 * Define a name functor for use with SimDataValue.
 * This creates a function named GetNAMEName() that returns "NAME".
 * @param NAME the name to return
 */
#define HAL_SIMDATAVALUE_DEFINE_NAME(NAME)                  \
  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr const char* \
      Get##NAME##Name() {                                   \
    return #NAME;                                           \
  }

/**
 * Define a standard C API for simulation data.
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, HAL_NotifyCallback callback, void* param,
 *      HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t uid)
 * - TYPE NS_GetCAPINAME(int32_t index)
 * - void NS_SetCAPINAME(int32_t index, TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param DATA the backing data array
 * @param LOWERNAME the lowercase name of the backing data variable
 */
#define HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, NS, CAPINAME, DATA, LOWERNAME)  \
  int32_t NS##_Register##CAPINAME##Callback(                               \
      int32_t index, HAL_NotifyCallback callback, void* param,             \
      HAL_Bool initialNotify) {                                            \
    return DATA[index].LOWERNAME.RegisterCallback(callback, param,         \
                                                  initialNotify);          \
  }                                                                        \
                                                                           \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t uid) {       \
    DATA[index].LOWERNAME.CancelCallback(uid);                             \
  }                                                                        \
                                                                           \
  TYPE NS##_Get##CAPINAME(int32_t index) { return DATA[index].LOWERNAME; } \
                                                                           \
  void NS##_Set##CAPINAME(int32_t index, TYPE LOWERNAME) {                 \
    DATA[index].LOWERNAME = LOWERNAME;                                     \
  }

/**
 * Define a standard C API for simulation data (channel variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, int32_t channel, HAL_NotifyCallback callback,
 *      void* param, HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t channel, int32_t uid)
 * - TYPE NS_GetCAPINAME(int32_t index, int32_t channel)
 * - void NS_SetCAPINAME(int32_t index, int32_t channel, TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param DATA the backing data array
 * @param LOWERNAME the lowercase name of the backing data variable array
 */
#define HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(TYPE, NS, CAPINAME, DATA,      \
                                             LOWERNAME)                     \
  int32_t NS##_Register##CAPINAME##Callback(                                \
      int32_t index, int32_t channel, HAL_NotifyCallback callback,          \
      void* param, HAL_Bool initialNotify) {                                \
    return DATA[index].LOWERNAME[channel].RegisterCallback(callback, param, \
                                                           initialNotify);  \
  }                                                                         \
                                                                            \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t channel,      \
                                       int32_t uid) {                       \
    DATA[index].LOWERNAME[channel].CancelCallback(uid);                     \
  }                                                                         \
                                                                            \
  TYPE NS##_Get##CAPINAME(int32_t index, int32_t channel) {                 \
    return DATA[index].LOWERNAME[channel];                                  \
  }                                                                         \
                                                                            \
  void NS##_Set##CAPINAME(int32_t index, int32_t channel, TYPE LOWERNAME) { \
    DATA[index].LOWERNAME[channel] = LOWERNAME;                             \
  }

/**
 * Define a standard C API for simulation data (no index variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t uid)
 * - TYPE NS_GetCAPINAME(void)
 * - void NS_SetCAPINAME(TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param DATA the backing data pointer
 * @param LOWERNAME the lowercase name of the backing data variable
 */
#define HAL_SIMDATAVALUE_DEFINE_CAPI_NOINDEX(TYPE, NS, CAPINAME, DATA,       \
                                             LOWERNAME)                      \
  int32_t NS##_Register##CAPINAME##Callback(                                 \
      HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {    \
    return DATA->LOWERNAME.RegisterCallback(callback, param, initialNotify); \
  }                                                                          \
                                                                             \
  void NS##_Cancel##CAPINAME##Callback(int32_t uid) {                        \
    DATA->LOWERNAME.CancelCallback(uid);                                     \
  }                                                                          \
                                                                             \
  TYPE NS##_Get##CAPINAME(void) { return DATA->LOWERNAME; }                  \
                                                                             \
  void NS##_Set##CAPINAME(TYPE LOWERNAME) { DATA->LOWERNAME = LOWERNAME; }

/**
 * Define a stub standard C API for simulation data.
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, HAL_NotifyCallback callback, void* param,
 *      HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t uid)
 * - TYPE NS_GetCAPINAME(int32_t index)
 * - void NS_SetCAPINAME(int32_t index, TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param RETURN what to return from the Get function
 */
#define HAL_SIMDATAVALUE_STUB_CAPI(TYPE, NS, CAPINAME, RETURN)        \
  int32_t NS##_Register##CAPINAME##Callback(                          \
      int32_t index, HAL_NotifyCallback callback, void* param,        \
      HAL_Bool initialNotify) {                                       \
    return 0;                                                         \
  }                                                                   \
                                                                      \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t uid) {} \
                                                                      \
  TYPE NS##_Get##CAPINAME(int32_t index) { return RETURN; }           \
                                                                      \
  void NS##_Set##CAPINAME(int32_t index, TYPE) {}

/**
 * Define a stub standard C API for simulation data (channel variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      int32_t index, int32_t channel, HAL_NotifyCallback callback,
 *      void* param, HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t index, int32_t channel, int32_t uid)
 * - TYPE NS_GetCAPINAME(int32_t index, int32_t channel)
 * - void NS_SetCAPINAME(int32_t index, int32_t channel, TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param RETURN what to return from the Get function
 */
#define HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(TYPE, NS, CAPINAME, RETURN)       \
  int32_t NS##_Register##CAPINAME##Callback(                                 \
      int32_t index, int32_t channel, HAL_NotifyCallback callback,           \
      void* param, HAL_Bool initialNotify) {                                 \
    return 0;                                                                \
  }                                                                          \
                                                                             \
  void NS##_Cancel##CAPINAME##Callback(int32_t index, int32_t channel,       \
                                       int32_t uid) {}                       \
                                                                             \
  TYPE NS##_Get##CAPINAME(int32_t index, int32_t channel) { return RETURN; } \
                                                                             \
  void NS##_Set##CAPINAME(int32_t index, int32_t channel, TYPE) {}

/**
 * Define a stub standard C API for simulation data (no index variant).
 *
 * Functions defined:
 * - int32 NS_RegisterCAPINAMECallback(
 *      HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify)
 * - void NS_CancelCAPINAMECallback(int32_t uid)
 * - TYPE NS_GetCAPINAME(void)
 * - void NS_SetCAPINAME(TYPE value)
 *
 * @param TYPE the underlying value type (e.g. double)
 * @param NS the "namespace" (e.g. HALSIM)
 * @param CAPINAME the C API name (usually first letter capitalized)
 * @param RETURN what to return from the Get function
 */
#define HAL_SIMDATAVALUE_STUB_CAPI_NOINDEX(TYPE, NS, CAPINAME, RETURN)    \
  int32_t NS##_Register##CAPINAME##Callback(                              \
      HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) { \
    return 0;                                                             \
  }                                                                       \
                                                                          \
  void NS##_Cancel##CAPINAME##Callback(int32_t uid) {}                    \
                                                                          \
  TYPE NS##_Get##CAPINAME(void) { return RETURN; }                        \
                                                                          \
  void NS##_Set##CAPINAME(TYPE) {}

}  // namespace hal
