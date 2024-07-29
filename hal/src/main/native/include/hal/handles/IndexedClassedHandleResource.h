// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <memory>
#include <vector>

#include <wpi/mutex.h>

#include "hal/Errors.h"
#include "hal/Types.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

/**
 * The IndexedClassedHandleResource class is a way to track handles. This
 * version
 * allows a limited number of handles that are allocated by index.
 * Because they are allocated by index, each individual index holds its own
 * mutex, which reduces contention heavily.]
 *
 * @tparam THandle The Handle Type (Must be typedefed from HAL_Handle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 * @tparam enumValue The type value stored in the handle
 *
 */
template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
class IndexedClassedHandleResource : public HandleBase {
  friend class IndexedClassedHandleResourceTest;

 public:
  IndexedClassedHandleResource() = default;
  IndexedClassedHandleResource(const IndexedClassedHandleResource&) = delete;
  IndexedClassedHandleResource& operator=(const IndexedClassedHandleResource&) =
      delete;

  THandle Allocate(int16_t index, std::shared_ptr<TStruct> toSet,
                   int32_t* status);
  int16_t GetIndex(THandle handle) {
    return getHandleTypedIndex(handle, enumValue, m_version);
  }
  std::shared_ptr<TStruct> Get(THandle handle);
  void Free(THandle handle);
  void ResetHandles() override;

 private:
  std::array<std::shared_ptr<TStruct>, size> m_structures;
  std::array<wpi::mutex, size> m_handleMutexes;
};

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
THandle
IndexedClassedHandleResource<THandle, TStruct, size, enumValue>::Allocate(
    int16_t index, std::shared_ptr<TStruct> toSet, int32_t* status) {
  // don't acquire the lock if we can fail early.
  if (index < 0 || index >= size) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_structures[index] != nullptr) {
    *status = RESOURCE_IS_ALLOCATED;
    return HAL_kInvalidHandle;
  }
  m_structures[index] = toSet;
  return static_cast<THandle>(hal::createHandle(index, enumValue, m_version));
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
std::shared_ptr<TStruct>
IndexedClassedHandleResource<THandle, TStruct, size, enumValue>::Get(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle);
  if (index < 0 || index >= size) {
    return nullptr;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // return structure. Null will propagate correctly, so no need to manually
  // check.
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
void IndexedClassedHandleResource<THandle, TStruct, size, enumValue>::Free(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle);
  if (index < 0 || index >= size) {
    return;
  }
  // lock and deallocated handle
  std::scoped_lock lock(m_handleMutexes[index]);
  m_structures[index].reset();
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
void IndexedClassedHandleResource<THandle, TStruct, size,
                                  enumValue>::ResetHandles() {
  for (int i = 0; i < size; i++) {
    std::scoped_lock lock(m_handleMutexes[i]);
    m_structures[i].reset();
  }
  HandleBase::ResetHandles();
}
}  // namespace hal
