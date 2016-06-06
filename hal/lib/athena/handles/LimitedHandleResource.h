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
#include "HAL/Handles.h"
#include "HAL/cpp/priority_mutex.h"
#include "HandlesInternal.h"

namespace hal {

constexpr int32_t LimitedResourceIndexOutOfRange = -1;
constexpr int32_t LimitedResourceNotAllocated = -2;

/**
 * The LimitedHandleResource class is a way to track handles. This version
 * allows a limited number of handles that are allocated sequentially.
 *
 * @tparam THandle The Handle Type (Must be typedefed from HalHandle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 * @tparam enumValue The type value stored in the handle
 *
 */
template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
class LimitedHandleResource {
  friend class LimitedHandleResourceTest;

 public:
  LimitedHandleResource(const LimitedHandleResource&) = delete;
  LimitedHandleResource operator=(const LimitedHandleResource&) = delete;
  LimitedHandleResource();
  TStruct* Allocate(const TStruct& toSet, THandle* handle);
  TStruct Get(THandle handle, int32_t* status);
  bool Free(THandle handle, TStruct* structure);

 private:
  TStruct m_structures[size];
  bool m_allocated[size];
  priority_mutex m_handleMutexes[size];
  priority_mutex m_allocateMutex;
};

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
LimitedHandleResource<THandle, TStruct, size,
                      enumValue>::LimitedHandleResource() {
  // ensure all allocations are initially false.
  for (int i = 0; i < size; i++) {
    m_allocated[i] = false;
  }
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
TStruct* LimitedHandleResource<THandle, TStruct, size, enumValue>::Allocate(
    const TStruct& toSet, THandle* handle) {
  // globally lock to loop through indices
  std::lock_guard<priority_mutex> sync(m_allocateMutex);
  int16_t i;
  for (i = 0; i < size; i++) {
    if (m_allocated[i] == false) {
      // if a false index is found, grab its specific mutex
      // and allocate it.
      std::lock_guard<priority_mutex> sync(m_handleMutexes[i]);
      m_allocated[i] = true;
      m_structures[i] = toSet;
      *handle = (THandle)createHandle(i, enumValue);
      return &m_structures[i];
    }
  }
  *handle = HAL_HANDLE_OUT_OF_HANDLES;
  return nullptr;
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
TStruct LimitedHandleResource<THandle, TStruct, size, enumValue>::Get(
    THandle handle, int32_t* status) {
  *status = 0;
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index >= size) {
    *status = PARAMETER_OUT_OF_RANGE;
    return TStruct();
  }
  std::lock_guard<priority_mutex> sync(m_handleMutexes[index]);
  // check for already deallocated handle, then return structure
  if (!m_allocated[index]) {
    *status = NO_AVAILABLE_RESOURCES;
    return TStruct();
  }
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
bool LimitedHandleResource<THandle, TStruct, size, enumValue>::Free(
    THandle handle, TStruct* structure) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index >= size) return false;  // not deallocated
  // lock and deallocated handle
  std::lock_guard<priority_mutex> sync(m_allocateMutex);
  std::lock_guard<priority_mutex> lock(m_handleMutexes[index]);
  if (m_allocated[index]) {
    *structure = m_structures[index];
    m_allocated[index] = false;
    return true;  // deallocated successfully, and resource passed back
  }
  return false;  // not deallocated successfully
}
}
