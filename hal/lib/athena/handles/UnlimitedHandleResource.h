/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
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

namespace hal {

/**
 * The UnlimitedHandleResource class is a way to track handles. This version
 * allows an unlimted number of handles that are allocated sequentially. When
 * possible, indices are reused to save memory usage and keep the array length
 * down.
 * However, automatic array management has not been implemented, but might be in
 * the future.
 * Because we have to loop through the allocator, we must use a global mutex.
 */
template <typename T, typename U, HalHandleEnum enumValue>
class UnlimitedHandleResource {
 public:
  UnlimitedHandleResource(const UnlimitedHandleResource&) = delete;
  UnlimitedHandleResource operator=(const UnlimitedHandleResource&) = delete;
  explicit UnlimitedHandleResource();
  T Allocate(std::shared_ptr<U> structure);
  std::shared_ptr<U> Get(T handle);
  void Free(T handle);

 private:
  std::vector<std::shared_ptr<U>> m_structures;
  priority_recursive_mutex m_handleMutex;
};

template <typename T, typename U, HalHandleEnum enumValue>
UnlimitedHandleResource<T, U, enumValue>::UnlimitedHandleResource() {
  m_structures = std::vector<std::shared_ptr<U>>();
}

template <typename T, typename U, HalHandleEnum enumValue>
T UnlimitedHandleResource<T, U, enumValue>::Allocate(
    std::shared_ptr<U> structure) {
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutex);
  int16_t i;
  for (i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] == nullptr) {
      m_structures[i] = structure;
      return (T)createHandle(i, enumValue);
    }
  }
  if (i >= INT16_MAX) return HAL_HANDLE_OUT_OF_HANDLES;

  m_structures.push_back(structure);
  return (T)createHandle(i, enumValue);
}

template <typename T, typename U, HalHandleEnum enumValue>
std::shared_ptr<U> UnlimitedHandleResource<T, U, enumValue>::Get(T handle) {
  int16_t index = getIndex(handle);
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutex);
  if (index < 0 || index > m_structures.size()) return nullptr;
  return m_structures[index];
}

template <typename T, typename U, HalHandleEnum enumValue>
void UnlimitedHandleResource<T, U, enumValue>::Free(T handle) {
  int16_t index = getIndex(handle);
  std::lock_guard<priority_recursive_mutex> sync(m_handleMutex);
  if (index < 0 || index > m_structures.size()) return nullptr;
  m_structures[index].reset();
}
}
