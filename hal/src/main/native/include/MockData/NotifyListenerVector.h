/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "NotifyListener.h"

namespace hal {
// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
class NotifyListenerVector {
  struct private_init {};

 public:
  typedef typename std::vector<NotifyListener>::size_type size_type;

  // Constructor for creating copies of the vector
  NotifyListenerVector(const NotifyListenerVector*, const private_init&);

  // Delete all default constructors so they cannot be used
  NotifyListenerVector& operator=(const NotifyListenerVector&) = delete;
  NotifyListenerVector() = delete;
  NotifyListenerVector(const NotifyListenerVector&) = delete;

  // Create a new vector with a single callback inside of it
  NotifyListenerVector(void* param, HAL_NotifyCallback callback,
                       unsigned int* newUid);

  size_type size() const { return m_vector.size(); }
  NotifyListener& operator[](size_type i) { return m_vector[i]; }
  const NotifyListener& operator[](size_type i) const { return m_vector[i]; }

  // Add a new NotifyListener to a copy of the vector.  If there are elements on
  // the freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  std::shared_ptr<NotifyListenerVector> emplace_back(
      void* param, HAL_NotifyCallback callback, unsigned int* newUid);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse. Returns a copy
  std::shared_ptr<NotifyListenerVector> erase(unsigned int uid);

 private:
  std::vector<NotifyListener> m_vector;
  std::vector<unsigned int> m_free;

  // Add a new NotifyListener to the vector.  If there are elements on the
  // freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  unsigned int emplace_back_impl(void* param, HAL_NotifyCallback callback);

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse.
  void erase_impl(unsigned int uid);
};

}  // namespace hal
