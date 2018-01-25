/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include <memory>
#include <queue>
#include <vector>

#include "NotifyListener.h"

namespace hal {
// Vector which provides an integrated freelist for removal and reuse of
// individual elements.

template <typename ListenerType>
class HalCallbackListenerVectorImpl {
  struct private_init {};

 public:
  typedef typename std::vector<HalCallbackListener<ListenerType>>::size_type
      size_type;

  // Constructor for creating copies of the vector
  HalCallbackListenerVectorImpl(const HalCallbackListenerVectorImpl* copyFrom,
                                const private_init&);

  // Delete all default constructors so they cannot be used
  HalCallbackListenerVectorImpl& operator=(
      const HalCallbackListenerVectorImpl&) = delete;
  HalCallbackListenerVectorImpl() = delete;
  HalCallbackListenerVectorImpl(const HalCallbackListenerVectorImpl&) = delete;

  // Create a new vector with a single callback inside of it
  HalCallbackListenerVectorImpl(void* param, ListenerType callback,
                                unsigned int* newUid) {
    *newUid = emplace_back_impl(param, callback);
  }

  size_type size() const { return m_vector.size(); }
  HalCallbackListener<ListenerType>& operator[](size_type i) {
    return m_vector[i];
  }
  const HalCallbackListener<ListenerType>& operator[](size_type i) const {
    return m_vector[i];
  }

  // Add a new NotifyListener to a copy of the vector.  If there are elements on
  // the freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  std::shared_ptr<HalCallbackListenerVectorImpl<ListenerType>> emplace_back(
      void* param, ListenerType callback, unsigned int* newUid);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse. Returns a copy
  std::shared_ptr<HalCallbackListenerVectorImpl<ListenerType>> erase(
      unsigned int uid);

 private:
  std::vector<HalCallbackListener<ListenerType>> m_vector;
  std::vector<unsigned int> m_free;

  // Add a new NotifyListener to the vector.  If there are elements on the
  // freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  unsigned int emplace_back_impl(void* param, ListenerType callback);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse.
  void erase_impl(unsigned int uid);
};

template <typename ListenerType>
HalCallbackListenerVectorImpl<ListenerType>::HalCallbackListenerVectorImpl(
    const HalCallbackListenerVectorImpl<ListenerType>* copyFrom,
    const private_init&)
    : m_vector(copyFrom->m_vector), m_free(copyFrom->m_free) {}

template <typename ListenerType>
std::shared_ptr<HalCallbackListenerVectorImpl<ListenerType>>
HalCallbackListenerVectorImpl<ListenerType>::emplace_back(
    void* param, ListenerType callback, unsigned int* newUid) {
  auto newVector =
      std::make_shared<HalCallbackListenerVectorImpl<ListenerType>>(
          this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  *newUid = newVector->emplace_back_impl(param, callback);
  return newVector;
}

template <typename ListenerType>
std::shared_ptr<HalCallbackListenerVectorImpl<ListenerType>>
HalCallbackListenerVectorImpl<ListenerType>::erase(unsigned int uid) {
  auto newVector =
      std::make_shared<HalCallbackListenerVectorImpl<ListenerType>>(
          this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  newVector->erase_impl(uid);
  return newVector;
}

template <typename ListenerType>
unsigned int HalCallbackListenerVectorImpl<ListenerType>::emplace_back_impl(
    void* param, ListenerType callback) {
  unsigned int uid;
  if (m_free.empty()) {
    uid = m_vector.size();
    m_vector.emplace_back(param, callback);
  } else {
    uid = m_free.back();
    m_free.pop_back();
    m_vector[uid] = HalCallbackListener<ListenerType>(param, callback);
  }
  return uid + 1;
}

template <typename ListenerType>
void HalCallbackListenerVectorImpl<ListenerType>::erase_impl(unsigned int uid) {
  --uid;
  if (uid >= m_vector.size() || !m_vector[uid]) return;
  m_free.push_back(uid);
  m_vector[uid] = HalCallbackListener<ListenerType>();
}

typedef HalCallbackListenerVectorImpl<HAL_NotifyCallback> NotifyListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_BufferCallback> BufferListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_ConstBufferCallback>
    ConstBufferListenerVector;

}  // namespace hal

#endif
