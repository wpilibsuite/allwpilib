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
 * mutex, which reduces contention heavily.
 */
template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
class IndexedHandleResource {
 public:
  IndexedHandleResource(const IndexedHandleResource&) = delete;
  IndexedHandleResource operator=(const IndexedHandleResource&) = delete;
  IndexedHandleResource();
  T Allocate(int16_t index);
  U* Get(T handle);
  void Free(T handle);

 private:
  U m_structures[size];
  bool m_allocated[size];
  priority_recursive_mutex m_handleMutexes[size];
};

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
IndexedHandleResource<T, U, size, enumValue>::IndexedHandleResource() {
  // ensure all allocations are initially false.
  for (int i = 0; i < size; i++) {
    m_allocated[i] = false;
  }
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
T IndexedHandleResource<T, U, size, enumValue>::Allocate(int16_t index) {
  // don't aquire the lock if we can fail early.
  if (index < 0 || index > size) return HAL_HANDLE_INDEX_OUT_OF_RANGE;
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_allocated[index]) return HAL_HANDLE_ALREADY_ALLOCATED;
  m_allocated[index] = true;
  return (T)hal::createHandle(index, enumValue);
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
U* IndexedHandleResource<T, U, size, enumValue>::Get(T handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return nullptr;
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  // check for already deallocated handle, then return structure
  if (!m_allocated[index]) return nullptr;
  return m_structures[index];
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
void IndexedHandleResource<T, U, size, enumValue>::Free(T handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return;
  // lock and deallocated handle
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  m_allocated(index) = false;
}
}
