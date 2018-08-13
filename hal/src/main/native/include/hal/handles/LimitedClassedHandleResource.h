/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <array>
#include <memory>

#include <wpi/mutex.h>

#include "hal/Types.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

/**
 * The LimitedClassedHandleResource class is a way to track handles. This
 * version
 * allows a limited number of handles that are allocated sequentially.
 *
 * @tparam THandle The Handle Type (Must be typedefed from HAL_Handle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 * @tparam enumValue The type value stored in the handle
 *
 */
template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
class LimitedClassedHandleResource : public HandleBase {
  friend class LimitedClassedHandleResourceTest;

 public:
  LimitedClassedHandleResource() = default;
  LimitedClassedHandleResource(const LimitedClassedHandleResource&) = delete;
  LimitedClassedHandleResource& operator=(const LimitedClassedHandleResource&) =
      delete;

  THandle Allocate(std::shared_ptr<TStruct> toSet);
  std::shared_ptr<TStruct> Get(THandle handle);
  void Free(THandle handle);
  void ResetHandles() override;

 private:
  std::array<std::shared_ptr<TStruct>, size> m_structures;
  std::array<wpi::mutex, size> m_handleMutexes;
  wpi::mutex m_allocateMutex;
};

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
THandle
LimitedClassedHandleResource<THandle, TStruct, size, enumValue>::Allocate(
    std::shared_ptr<TStruct> toSet) {
  // globally lock to loop through indices
  std::lock_guard<wpi::mutex> lock(m_allocateMutex);
  for (int16_t i = 0; i < size; i++) {
    if (m_structures[i] == nullptr) {
      // if a false index is found, grab its specific mutex
      // and allocate it.
      std::lock_guard<wpi::mutex> lock(m_handleMutexes[i]);
      m_structures[i] = toSet;
      return static_cast<THandle>(createHandle(i, enumValue, m_version));
    }
  }
  return HAL_kInvalidHandle;
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
std::shared_ptr<TStruct>
LimitedClassedHandleResource<THandle, TStruct, size, enumValue>::Get(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue, m_version);
  if (index < 0 || index >= size) {
    return nullptr;
  }
  std::lock_guard<wpi::mutex> lock(m_handleMutexes[index]);
  // return structure. Null will propogate correctly, so no need to manually
  // check.
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
void LimitedClassedHandleResource<THandle, TStruct, size, enumValue>::Free(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue, m_version);
  if (index < 0 || index >= size) return;
  // lock and deallocated handle
  std::lock_guard<wpi::mutex> allocateLock(m_allocateMutex);
  std::lock_guard<wpi::mutex> handleLock(m_handleMutexes[index]);
  m_structures[index].reset();
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue>
void LimitedClassedHandleResource<THandle, TStruct, size,
                                  enumValue>::ResetHandles() {
  {
    std::lock_guard<wpi::mutex> allocateLock(m_allocateMutex);
    for (int i = 0; i < size; i++) {
      std::lock_guard<wpi::mutex> handleLock(m_handleMutexes[i]);
      m_structures[i].reset();
    }
  }
  HandleBase::ResetHandles();
}
}  // namespace hal
