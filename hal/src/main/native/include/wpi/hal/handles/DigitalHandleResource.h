// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <memory>

#include <wpi/mutex.h>

#include "hal/Errors.h"
#include "hal/Types.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

/**
 * The DigitalHandleResource class is a way to track handles. This version
 * allows a limited number of handles that are allocated by index.
 * The enum value is separate, as 2 enum values are allowed per handle
 * Because they are allocated by index, each individual index holds its own
 * mutex, which reduces contention heavily.
 *
 * @tparam THandle The Handle Type (Must be typedefed from HAL_Handle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 *
 */
template <typename THandle, typename TStruct, int16_t size>
class DigitalHandleResource : public HandleBase {
  friend class DigitalHandleResourceTest;

 public:
  DigitalHandleResource() = default;
  DigitalHandleResource(const DigitalHandleResource&) = delete;
  DigitalHandleResource& operator=(const DigitalHandleResource&) = delete;

  std::shared_ptr<TStruct> Allocate(int16_t index, HAL_HandleEnum enumValue,
                                    THandle* handle, int32_t* status);
  int16_t GetIndex(THandle handle, HAL_HandleEnum enumValue) {
    return getHandleTypedIndex(handle, enumValue, m_version);
  }
  std::shared_ptr<TStruct> Get(THandle handle, HAL_HandleEnum enumValue);
  void Free(THandle handle, HAL_HandleEnum enumValue);
  void ResetHandles() override;

 private:
  std::array<std::shared_ptr<TStruct>, size> m_structures;
  std::array<wpi::mutex, size> m_handleMutexes;
};

template <typename THandle, typename TStruct, int16_t size>
std::shared_ptr<TStruct>
DigitalHandleResource<THandle, TStruct, size>::Allocate(
    int16_t index, HAL_HandleEnum enumValue, THandle* handle, int32_t* status) {
  // don't acquire the lock if we can fail early.
  if (index < 0 || index >= size) {
    *handle = HAL_kInvalidHandle;
    *status = RESOURCE_OUT_OF_RANGE;
    return nullptr;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_structures[index] != nullptr) {
    *handle = HAL_kInvalidHandle;
    *status = RESOURCE_IS_ALLOCATED;
    return m_structures[index];
  }
  m_structures[index] = std::make_shared<TStruct>();
  *handle =
      static_cast<THandle>(hal::createHandle(index, enumValue, m_version));
  *status = HAL_SUCCESS;
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size>
std::shared_ptr<TStruct> DigitalHandleResource<THandle, TStruct, size>::Get(
    THandle handle, HAL_HandleEnum enumValue) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle, enumValue);
  if (index < 0 || index >= size) {
    return nullptr;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // return structure. Null will propagate correctly, so no need to manually
  // check.
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size>
void DigitalHandleResource<THandle, TStruct, size>::Free(
    THandle handle, HAL_HandleEnum enumValue) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle, enumValue);
  if (index < 0 || index >= size) {
    return;
  }
  // lock and deallocated handle
  std::scoped_lock lock(m_handleMutexes[index]);
  m_structures[index].reset();
}

template <typename THandle, typename TStruct, int16_t size>
void DigitalHandleResource<THandle, TStruct, size>::ResetHandles() {
  for (int i = 0; i < size; i++) {
    std::scoped_lock lock(m_handleMutexes[i]);
    m_structures[i].reset();
  }
  HandleBase::ResetHandles();
}
}  // namespace hal
