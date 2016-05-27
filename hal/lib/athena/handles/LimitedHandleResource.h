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
 * The LimitedHandleResource class is a way to track handles. This version
 * allows a limited number of handles that are allocated sequentially.
 */
template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
class LimitedHandleResource {
 public:
  LimitedHandleResource(const LimitedHandleResource&) = delete;
  LimitedHandleResource operator=(const LimitedHandleResource&) = delete;
  LimitedHandleResource();
  T Allocate();
  U* Get(T handle);
  void Free(T handle);

 private:
  U m_structures[size];
  bool m_allocated[size];
  priority_recursive_mutex m_handleMutexes[size];
  priority_recursive_mutex m_allocateMutex;
};

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
LimitedHandleResource<T, U, size, enumValue>::LimitedHandleResource() {
  // ensure all allocations are initially false.
  for (int i = 0; i < size; i++) {
    m_allocated[i] = false;
  }
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
T LimitedHandleResource<T, U, size, enumValue>::Allocate() {
  // globally lock to loop through indices
  std::lock_guard<priority_recursive_mutex> sync(m_allocateMutex);
  int16_t i;
  for (i = 0; i < size; i++) {
    if (m_allocated[i] == false) {
      // if a false index is found, grab its specific mutex
      // and allocate it.
      std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[i]);
      m_allocated[i] = true;
      return (T)createHandle(i, enumValue);
    }
  }
  return HAL_HANDLE_OUT_OF_HANDLES;
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
U* LimitedHandleResource<T, U, size, enumValue>::Get(T handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return nullptr;
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutexes[index]);
  // check for already deallocated handle, then return structure
  if (!m_allocated[index]) return nullptr;
  return m_structures[index];
}

template <typename T, typename U, int16_t size, HalHandleEnum enumValue>
void LimitedHandleResource<T, U, size, enumValue>::Free(T handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = getHandleTypedIndex(handle, enumValue);
  if (index < 0 || index > size) return nullptr;
  // lock and deallocated handle
  std::lock_guard<priority_recursive_mutex> sync(m_allocateMutex);
  std::lock_guard<priority_recursive_mutex> lock(m_handleMutexes[index]);
  m_allocated[index] = false;
}
}
