// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <memory>
#include <string_view>
#include <utility>

#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/util/expected"
#include "wpi/util/mutex.hpp"

namespace wpi::hal {

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

  wpi::util::expected<std::pair<THandle, std::shared_ptr<TStruct>>, HAL_Status>
  Allocate(int16_t index, HAL_HandleEnum enumValue, std::string_view name);
  int16_t GetIndex(THandle handle, HAL_HandleEnum enumValue) {
    return getHandleTypedIndex(handle, enumValue, m_version);
  }
  std::shared_ptr<TStruct> Get(THandle handle, HAL_HandleEnum enumValue);
  void Free(THandle handle, HAL_HandleEnum enumValue);
  void ResetHandles() override;

 private:
  std::array<std::shared_ptr<TStruct>, size> m_structures;
  std::array<wpi::util::mutex, size> m_handleMutexes;
};

template <typename THandle, typename TStruct, int16_t size>
wpi::util::expected<std::pair<THandle, std::shared_ptr<TStruct>>, HAL_Status>
DigitalHandleResource<THandle, TStruct, size>::Allocate(
    int16_t index, HAL_HandleEnum enumValue, std::string_view name) {
  // don't acquire the lock if we can fail early.
  if (index < 0 || index >= size) {
    return wpi::util::unexpected(MakeErrorIndexOutOfRange(
        HAL_RESOURCE_OUT_OF_RANGE, name, 0, size, index));
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_structures[index] != nullptr) {
    if constexpr (detail::HasPreviousAllocation<TStruct>) {
      return wpi::util::unexpected(MakeErrorPreviouslyAllocated(
          HAL_RESOURCE_IS_ALLOCATED, name, index,
          m_structures[index]->previousAllocation));
    } else {
      return wpi::util::unexpected(MakeErrorPreviouslyAllocated(
          HAL_RESOURCE_IS_ALLOCATED, name, index, "unknown"));
    }
  }
  m_structures[index] = std::make_shared<TStruct>();
  THandle handle =
      static_cast<THandle>(wpi::hal::createHandle(index, enumValue, m_version));
  return std::pair{handle, m_structures[index]};
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
}  // namespace wpi::hal
