// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <utility>
#include <vector>

#include <wpi/mutex.h>

#include "hal/Types.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

/**
 * The UnlimitedHandleResource class is a way to track handles. This version
 * allows an unlimted number of handles that are allocated sequentially. When
 * possible, indices are reused to save memory usage and keep the array length
 * down.
 * However, automatic array management has not been implemented, but might be in
 * the future.
 * Because we have to loop through the allocator, we must use a global mutex.

 * @tparam THandle The Handle Type (Must be typedefed from HAL_Handle)
 * @tparam TStruct The struct type held by this resource
 * @tparam enumValue The type value stored in the handle
 *
 */
template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
class UnlimitedHandleResource : public HandleBase {
  friend class UnlimitedHandleResourceTest;

 public:
  UnlimitedHandleResource() = default;
  UnlimitedHandleResource(const UnlimitedHandleResource&) = delete;
  UnlimitedHandleResource& operator=(const UnlimitedHandleResource&) = delete;

  THandle Allocate(std::shared_ptr<TStruct> structure);
  int16_t GetIndex(THandle handle) {
    return getHandleTypedIndex(handle, enumValue, m_version);
  }
  std::shared_ptr<TStruct> Get(THandle handle);
  /* Returns structure previously at that handle (or nullptr if none) */
  std::shared_ptr<TStruct> Free(THandle handle);
  void ResetHandles() override;

  /* Calls func(THandle, TStruct*) for each handle.  Note this holds the
   * global lock for the entirety of execution.
   */
  template <typename Functor>
  void ForEach(Functor func);

 private:
  std::vector<std::shared_ptr<TStruct>> m_structures;
  wpi::mutex m_handleMutex;
};

template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
THandle UnlimitedHandleResource<THandle, TStruct, enumValue>::Allocate(
    std::shared_ptr<TStruct> structure) {
  std::scoped_lock lock(m_handleMutex);
  size_t i;
  for (i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] == nullptr) {
      m_structures[i] = structure;
      return static_cast<THandle>(createHandle(i, enumValue, m_version));
    }
  }
  if (i >= INT16_MAX) {
    return HAL_kInvalidHandle;
  }

  m_structures.push_back(structure);
  return static_cast<THandle>(
      createHandle(static_cast<int16_t>(i), enumValue, m_version));
}

template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
std::shared_ptr<TStruct>
UnlimitedHandleResource<THandle, TStruct, enumValue>::Get(THandle handle) {
  int16_t index = GetIndex(handle);
  std::scoped_lock lock(m_handleMutex);
  if (index < 0 || index >= static_cast<int16_t>(m_structures.size())) {
    return nullptr;
  }
  return m_structures[index];
}

template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
std::shared_ptr<TStruct>
UnlimitedHandleResource<THandle, TStruct, enumValue>::Free(THandle handle) {
  int16_t index = GetIndex(handle);
  std::scoped_lock lock(m_handleMutex);
  if (index < 0 || index >= static_cast<int16_t>(m_structures.size())) {
    return nullptr;
  }
  return std::move(m_structures[index]);
}

template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
void UnlimitedHandleResource<THandle, TStruct, enumValue>::ResetHandles() {
  {
    std::scoped_lock lock(m_handleMutex);
    for (size_t i = 0; i < m_structures.size(); i++) {
      m_structures[i].reset();
    }
  }
  HandleBase::ResetHandles();
}

template <typename THandle, typename TStruct, HAL_HandleEnum enumValue>
template <typename Functor>
void UnlimitedHandleResource<THandle, TStruct, enumValue>::ForEach(
    Functor func) {
  std::scoped_lock lock(m_handleMutex);
  size_t i;
  for (i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] != nullptr) {
      func(static_cast<THandle>(createHandle(i, enumValue, m_version)),
           m_structures[i].get());
    }
  }
}

}  // namespace hal
