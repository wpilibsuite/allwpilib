/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_UIDVECTOR_H_
#define WPIUTIL_SUPPORT_UIDVECTOR_H_

#include <utility>
#include <vector>

namespace wpi {

// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
// @tparam T element type; must be default-constructible and evaluate in
//           boolean context to false when "empty"
// @tparam reuse_threshold how many free elements to store up before starting
//                         to recycle them
template <typename T, typename std::vector<T>::size_type reuse_threshold>
class UidVector {
 public:
  typedef typename std::vector<T>::size_type size_type;

  bool empty() const { return m_active_count == 0; }
  size_type size() const { return m_vector.size(); }
  T& operator[](size_type i) { return m_vector[i]; }
  const T& operator[](size_type i) const { return m_vector[i]; }

  // Add a new T to the vector.  If there are elements on the freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index.
  template <class... Args>
  size_type emplace_back(Args&&... args) {
    size_type uid;
    if (m_free.size() < reuse_threshold) {
      uid = m_vector.size();
      m_vector.emplace_back(std::forward<Args>(args)...);
    } else {
      uid = m_free.front();
      m_free.erase(m_free.begin());
      m_vector[uid] = T(std::forward<Args>(args)...);
    }
    ++m_active_count;
    return uid;
  }

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse.
  void erase(size_type uid) {
    if (uid >= m_vector.size() || !m_vector[uid]) return;
    m_free.push_back(uid);
    m_vector[uid] = T();
    --m_active_count;
  }

 private:
  std::vector<T> m_vector;
  std::vector<size_type> m_free;
  size_type m_active_count{0};
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_UIDVECTOR_H_
