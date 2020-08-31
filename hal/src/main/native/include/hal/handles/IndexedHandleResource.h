/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <array>
#include <memory>

#include <wpi/mutex.h>

#include "hal/Errors.h"
#include "hal/Types.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

/**
 * The IndexedHandleResource class is a way to track handles. This version
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
          HAL_HandleEnum enumValue, uint16_t maxRefCount>
class IndexedHandleResource : public HandleBase {
  static_assert(maxRefCount > 0, "Must have a max ref count of at least 1");
  friend class IndexedHandleResourceTest;

 public:
  IndexedHandleResource() = default;
  IndexedHandleResource(const IndexedHandleResource&) = delete;
  IndexedHandleResource& operator=(const IndexedHandleResource&) = delete;

  template<typename TInitializer>
  int32_t Allocate(int16_t index, THandle* handle, std::shared_ptr<TStruct>* resource, TInitializer initialConstructor);

  int16_t GetIndex(THandle handle) {
    return getHandleTypedIndex(handle, enumValue, m_version);
  }
  std::shared_ptr<TStruct> Get(THandle handle);
  std::shared_ptr<TStruct> Free(THandle handle);
  void ResetHandles() override;

 private:
  std::array<std::pair<std::shared_ptr<TStruct>, uint16_t>, size> m_structures;
  std::array<wpi::mutex, size> m_handleMutexes;
};

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue, uint16_t maxRefCount>
template <typename TInitializer>
int32_t IndexedHandleResource<THandle, TStruct, size, enumValue, maxRefCount>::Allocate(
    int16_t index, THandle* handle, std::shared_ptr<TStruct>* resource, TInitializer initialConstructor) {
  // don't aquire the lock if we can fail early.
  if (index < 0 || index >= size) {
    return RESOURCE_OUT_OF_RANGE;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // check for allocation, otherwise allocate and return a valid handle
  if (m_structures[index].first != nullptr) {
    if (m_structures[index].second >= maxRefCount) {
      return RESOURCE_IS_ALLOCATED;
    }
    m_structures[index].second++;
  } else {
    m_structures[index].first = std::make_shared<TStruct>();
    m_structures[index].second = 1;
    initialConstructor(m_structures[index].first.get());
  }

  *handle = static_cast<THandle>(hal::createHandle(index, enumValue, m_version));
  if (resource) {
    *resource = m_structures[index].first;
  }
  return 0;
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue, uint16_t maxRefCount>
std::shared_ptr<TStruct>
IndexedHandleResource<THandle, TStruct, size, enumValue, maxRefCount>::Get(THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle);
  if (index < 0 || index >= size) {
    return nullptr;
  }
  std::scoped_lock lock(m_handleMutexes[index]);
  // return structure. Null will propogate correctly, so no need to manually
  // check.
  return m_structures[index].first;
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue, uint16_t maxRefCount>
std::shared_ptr<TStruct> IndexedHandleResource<THandle, TStruct, size, enumValue, maxRefCount>::Free(
    THandle handle) {
  // get handle index, and fail early if index out of range or wrong handle
  int16_t index = GetIndex(handle);
  if (index < 0 || index >= size) return nullptr;
  // lock and deallocated handle
  std::scoped_lock lock(m_handleMutexes[index]);
  if (m_structures[index].first == nullptr) {
    return nullptr;
  }
  std::shared_ptr<TStruct> toReturn = nullptr;
  auto oldRefCount = m_structures[index].second--;
  if (oldRefCount == 1) {
    toReturn = std::move(m_structures[index].first);
  }
  return toReturn;
}

template <typename THandle, typename TStruct, int16_t size,
          HAL_HandleEnum enumValue, uint16_t maxRefCount>
void IndexedHandleResource<THandle, TStruct, size, enumValue, maxRefCount>::ResetHandles() {
  for (int i = 0; i < size; i++) {
    std::scoped_lock lock(m_handleMutexes[i]);
    m_structures[i].first.reset();
  }
  HandleBase::ResetHandles();
}
}  // namespace hal
