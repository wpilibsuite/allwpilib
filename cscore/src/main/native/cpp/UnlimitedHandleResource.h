/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_UNLIMITEDHANDLERESOURCE_H_
#define CSCORE_UNLIMITEDHANDLERESOURCE_H_

#include <memory>
#include <utility>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

namespace cs {

// The UnlimitedHandleResource class is a way to track handles. This version
// allows an unlimted number of handles that are allocated sequentially. When
// possible, indices are reused to save memory usage and keep the array length
// down.
// However, automatic array management has not been implemented, but might be in
// the future.
// Because we have to loop through the allocator, we must use a global mutex.
//
// THandle needs to have the following attributes:
//  Type : enum or typedef
//  kIndexMax : static, constexpr, or enum value for the maximum index value
//  int GetTypedIndex() const : function that returns the index of the handle
//  THandle(int index, HandleType[int] type) : constructor for index and type
//
// @tparam THandle The Handle Type
// @tparam TStruct The struct type held by this resource
// @tparam typeValue The type value stored in the handle
// @tparam TMutex The mutex type to use
template <typename THandle, typename TStruct, int typeValue,
          typename TMutex = wpi::mutex>
class UnlimitedHandleResource {
 public:
  UnlimitedHandleResource(const UnlimitedHandleResource&) = delete;
  UnlimitedHandleResource operator=(const UnlimitedHandleResource&) = delete;
  UnlimitedHandleResource() = default;

  template <typename... Args>
  THandle Allocate(Args&&... args);
  THandle Allocate(std::shared_ptr<THandle> structure);

  std::shared_ptr<TStruct> Get(THandle handle);

  std::shared_ptr<TStruct> Free(THandle handle);

  template <typename T>
  wpi::ArrayRef<T> GetAll(wpi::SmallVectorImpl<T>& vec);

  std::vector<std::shared_ptr<TStruct>> FreeAll();

  // @param func functor with (THandle, const TStruct&) parameters
  template <typename F>
  void ForEach(F func);

  // @pram func functor with (const TStruct&) parameter and bool return value
  template <typename F>
  std::pair<THandle, std::shared_ptr<TStruct>> FindIf(F func);

 private:
  THandle MakeHandle(size_t i) {
    return THandle{static_cast<int>(i),
                   static_cast<typename THandle::Type>(typeValue)};
  }
  std::vector<std::shared_ptr<TStruct>> m_structures;
  TMutex m_handleMutex;
};

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
template <typename... Args>
THandle UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::Allocate(
    Args&&... args) {
  std::lock_guard<TMutex> sync(m_handleMutex);
  size_t i;
  for (i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] == nullptr) {
      m_structures[i] = std::make_shared<TStruct>(std::forward<Args>(args)...);
      return MakeHandle(i);
    }
  }
  if (i >= THandle::kIndexMax) return 0;

  m_structures.emplace_back(
      std::make_shared<TStruct>(std::forward<Args>(args)...));
  return MakeHandle(i);
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
THandle UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::Allocate(
    std::shared_ptr<THandle> structure) {
  std::lock_guard<TMutex> sync(m_handleMutex);
  size_t i;
  for (i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] == nullptr) {
      m_structures[i] = structure;
      return MakeHandle(i);
    }
  }
  if (i >= THandle::kIndexMax) return 0;

  m_structures.push_back(structure);
  return MakeHandle(i);
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
inline std::shared_ptr<TStruct>
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::Get(
    THandle handle) {
  auto index =
      handle.GetTypedIndex(static_cast<typename THandle::Type>(typeValue));
  if (index < 0) return nullptr;
  std::lock_guard<TMutex> sync(m_handleMutex);
  if (index >= static_cast<int>(m_structures.size())) return nullptr;
  return m_structures[index];
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
inline std::shared_ptr<TStruct>
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::Free(
    THandle handle) {
  auto index =
      handle.GetTypedIndex(static_cast<typename THandle::Type>(typeValue));
  if (index < 0) return nullptr;
  std::lock_guard<TMutex> sync(m_handleMutex);
  if (index >= static_cast<int>(m_structures.size())) return nullptr;
  auto rv = std::move(m_structures[index]);
  m_structures[index].reset();
  return rv;
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
template <typename T>
inline wpi::ArrayRef<T>
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::GetAll(
    wpi::SmallVectorImpl<T>& vec) {
  ForEach([&](THandle handle, const TStruct& data) { vec.push_back(handle); });
  return vec;
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
inline std::vector<std::shared_ptr<TStruct>>
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::FreeAll() {
  std::lock_guard<TMutex> sync(m_handleMutex);
  auto rv = std::move(m_structures);
  m_structures.clear();
  return rv;
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
template <typename F>
inline void
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::ForEach(F func) {
  std::lock_guard<TMutex> sync(m_handleMutex);
  for (size_t i = 0; i < m_structures.size(); i++) {
    if (m_structures[i] != nullptr) func(MakeHandle(i), *(m_structures[i]));
  }
}

template <typename THandle, typename TStruct, int typeValue, typename TMutex>
template <typename F>
inline std::pair<THandle, std::shared_ptr<TStruct>>
UnlimitedHandleResource<THandle, TStruct, typeValue, TMutex>::FindIf(F func) {
  std::lock_guard<TMutex> sync(m_handleMutex);
  for (size_t i = 0; i < m_structures.size(); i++) {
    auto& structure = m_structures[i];
    if (structure != nullptr && func(*structure))
      return std::make_pair(MakeHandle(i), structure);
  }
  return std::make_pair(0, nullptr);
}

}  // namespace cs

#endif  // CSCORE_UNLIMITEDHANDLERESOURCE_H_
