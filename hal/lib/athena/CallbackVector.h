/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace hal {
// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
template <typename T>
class CallbackVector {
  struct private_init {};

 public:
  typedef typename std::vector<T>::size_type size_type;

  // Constructor for creating copies of the vector
  CallbackVector(const CallbackVector* copyFrom, const private_init&);

  // Delete all default constructors so they cannot be used
  CallbackVector& operator=(const CallbackVector&) = delete;
  CallbackVector() = delete;
  CallbackVector(const CallbackVector&) = delete;

  // Create a new vector with a single callback inside of it
  CallbackVector(T callback, unsigned int* newUid);

  size_type size() const { return m_vector.size(); }
  T& operator[](size_type i) { return m_vector[i]; }
  const T& operator[](size_type i) const { return m_vector[i]; }

  // Add a new T to a copy of the vector.  If there are elements on the
  // freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  std::shared_ptr<CallbackVector> emplace_back(T callback,
                                               unsigned int* newUid);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse. Returns a copy
  std::shared_ptr<CallbackVector> erase(unsigned int uid);

 private:
  std::vector<T> m_vector;
  std::vector<unsigned int> m_free;

  // Add a new T to the vector.  If there are elements on the freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  unsigned int emplace_back_impl(T callback);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse.
  void erase_impl(unsigned int uid);
};

template <typename T>
CallbackVector<T>::CallbackVector(const CallbackVector* copyFrom,
                                  const private_init&)
    : m_vector(copyFrom->m_vector), m_free(copyFrom->m_free) {}

template <typename T>
CallbackVector<T>::CallbackVector(T callback, unsigned int* newUid) {
  *newUid = emplace_back_impl(callback);
}

template <typename T>
std::shared_ptr<CallbackVector<T>> CallbackVector<T>::emplace_back(
    T callback, unsigned int* newUid) {
  auto newVector = std::make_shared<CallbackVector<T>>(this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  *newUid = newVector->emplace_back_impl(callback);
  return newVector;
}

template <typename T>
std::shared_ptr<CallbackVector<T>> CallbackVector<T>::erase(unsigned int uid) {
  auto newVector = std::make_shared<CallbackVector<T>>(this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  newVector->erase_impl(uid);
  return newVector;
}

template <typename T>
unsigned int CallbackVector<T>::emplace_back_impl(T callback) {
  unsigned int uid;
  if (m_free.empty()) {
    uid = m_vector.size();
    m_vector.emplace_back(callback);
  } else {
    uid = m_free.back();
    m_free.pop_back();
    m_vector[uid] = callback;
  }
  return uid + 1;
}

template <typename T>
void CallbackVector<T>::erase_impl(unsigned int uid) {
  --uid;
  if (uid >= m_vector.size() || !m_vector[uid]) return;
  m_free.push_back(uid);
  m_vector[uid] = T();
}

}  // namespace hal
