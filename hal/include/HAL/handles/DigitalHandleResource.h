/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "HAL/Errors.h"
#include "HAL/Types.h"
#include "HAL/cpp/make_unique.h"
#include "HAL/cpp/priority_mutex.h"
#include "HAL/handles/HandlesInternal.h"

namespace hal {

/**
 * The DigitalHandleResource class is a way to track handles. This version
 * allows a limited number of handles that are allocated by index.
 * The enum value is seperate, as 2 enum values are allowed per handle
 * Because they are allocated by index, each individual index holds its own
 * mutex, which reduces contention heavily.]
 *
 * @tparam THandle The Handle Type (Must be typedefed from HAL_Handle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 *
 */
template <typename THandle, typename TStruct, int16_t size>
class DigitalHandleResource {
  friend class DigitalHandleResourceTest;

 public:
  DigitalHandleResource(const DigitalHandleResource&) = delete;
  DigitalHandleResource operator=(const DigitalHandleResource&) = delete;
  DigitalHandleResource();
  THandle Allocate(int16_t index, HAL_HandleEnum enumValue, int32_t* status);
  std::shared_ptr<TStruct> Get(THandle handle, HAL_HandleEnum enumValue);
  void Free(THandle handle, HAL_HandleEnum enumValue);

 private:
  // Dynamic array to shrink HAL file size.
  std::unique_ptr<std::shared_ptr<TStruct>[]> m_structures;
  std::unique_ptr<priority_mutex[]> m_handleMutexes;
};

template <typename THandle, typename TStruct, int16_t size>
DigitalHandleResource<THandle, TStruct, size>::DigitalHandleResource() {
  m_structures = std::make_unique<std::shared_ptr<TStruct>[]>(size);
  m_handleMutexes = std::make_unique<priority_mutex[]>(size);
}

template <typename THandle, typename TStruct, int16_t size>
THandle DigitalHandleResource<THandle, TStruct, size>::Allocate(
    int16_t index, HAL_HandleEnum enumValue, int32_t* status) {
  // don't aquire the lock if we can fail early.
  if (index < 0 || index >= size) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  std::lock_guard<priority_mutex> sync(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_structures[index] != nullptr) {
    *status = RESOURCE_IS_ALLOCATED;
    return HAL_kInvalidHandle;
  }
  m_structures[index] = std::make_shared<TStruct>();
  return static_cast<THandle>(hal::createHandle(index, enumValue));
}

template <typename THandle, typename TStruct, int16_t size>
std::shared_ptr<TStruct> DigitalHandleResource<THandle, TStruct, size>::Get(
    THandle handle, HAL_HandleEnum enumValue) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index >= size) {
    return nullptr;
  }
  std::lock_guard<priority_mutex> sync(m_handleMutexes[index]);
  // return structure. Null will propogate correctly, so no need to manually
  // check.
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size>
void DigitalHandleResource<THandle, TStruct, size>::Free(
    THandle handle, HAL_HandleEnum enumValue) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index >= size) return;
  // lock and deallocated handle
  std::lock_guard<priority_mutex> sync(m_handleMutexes[index]);
  m_structures[index].reset();
}
}  // namespace hal
