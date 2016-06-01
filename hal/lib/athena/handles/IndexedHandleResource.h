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

#include "HAL/Handles.h"
#include "HAL/cpp/priority_mutex.h"
#include "HandlesInternal.h"

namespace hal {

/**
 * The IndexedHandleResource class is a way to track handles. This version
 * allows a limited number of handles that are allocated by index.
 * Because they are allocated by index, each individual index holds its own
 * mutex, which reduces contention heavily.]
 *
 * @tparam THandle The Handle Type (Must be typedefed from HalHandle)
 * @tparam TStruct The struct type held by this resource
 * @tparam size The number of resources allowed to be allocated
 * @tparam enumValue The type value stored in the handle
 *
 */
template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
class IndexedHandleResource {
 public:
  IndexedHandleResource(const IndexedHandleResource&) = delete;
  IndexedHandleResource operator=(const IndexedHandleResource&) = delete;
  IndexedHandleResource();
  THandle Allocate(int16_t index, const TStruct& toSet);
  TStruct Get(THandle handle);
  void Free(THandle handle);

 private:
  TStruct m_structures[size];
  bool m_allocated[size];
  priority_recursive_mutex m_handleMutexes[size];
};

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
IndexedHandleResource<THandle, TStruct, size,
                      enumValue>::IndexedHandleResource() {
  // ensure all allocations are initially false.
  for (int i = 0; i < size; i++) {
    m_allocated[i] = false;
  }
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
THandle IndexedHandleResource<THandle, TStruct, size, enumValue>::Allocate(
    int16_t index, const TStruct& toSet) {
  // don't aquire the lock if we can fail early.
  if (index < 0 || index > size) return HAL_HANDLE_INDEX_OUT_OF_RANGE;
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_allocated[index]) return HAL_HANDLE_ALREADY_ALLOCATED;
  m_allocated[index] = true;
  m_structures[index] = toSet;
  return (THandle)hal::createHandle(index, enumValue);
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
TStruct IndexedHandleResource<THandle, TStruct, size, enumValue>::Get(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return nullptr;
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  // check for already deallocated handle, then return structure
  if (!m_allocated[index]) return nullptr;
  return m_structures[index];
}

template <typename THandle, typename TStruct, int16_t size,
          HalHandleEnum enumValue>
void IndexedHandleResource<THandle, TStruct, size, enumValue>::Free(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return;
  // lock and deallocated handle
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  m_allocated(index) = false;
}
}
