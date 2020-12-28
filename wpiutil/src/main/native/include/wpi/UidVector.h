// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UIDVECTOR_H_
#define WPIUTIL_WPI_UIDVECTOR_H_

#include <iterator>
#include <utility>
#include <vector>

namespace wpi {

namespace impl {
template <typename It>
class UidVectorIterator {
 public:
  using iterator_type = std::forward_iterator_tag;
  using value_type = typename It::value_type;
  using difference_type = typename It::difference_type;
  using reference = typename It::reference;
  using pointer = typename It::pointer;

  UidVectorIterator() = default;
  explicit UidVectorIterator(It it, It end) : m_it(it), m_end(end) {
    // advance to first non-empty element
    while (m_it != m_end && !*m_it) {
      ++m_it;
    }
  }

  reference operator*() const noexcept { return *m_it; }
  pointer operator->() const noexcept { return m_it.operator->(); }

  UidVectorIterator& operator++() noexcept {
    // advance past empty elements
    do {
      ++m_it;
    } while (m_it != m_end && !*m_it);
    return *this;
  }

  UidVectorIterator operator++(int) noexcept {
    UidVectorIterator it = *this;
    ++it;
    return it;
  }

  bool operator==(const UidVectorIterator& oth) const noexcept {
    return m_it == oth.m_it;
  }

  bool operator!=(const UidVectorIterator& oth) const noexcept {
    return m_it != oth.m_it;
  }

 private:
  It m_it;
  It m_end;
};
}  // namespace impl

// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
// @tparam T element type; must be default-constructible and evaluate in
//           boolean context to false when "empty"
// @tparam reuse_threshold how many free elements to store up before starting
//                         to recycle them
template <typename T, typename std::vector<T>::size_type reuse_threshold>
class UidVector {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = typename std::vector<T>::size_type;
  using difference_type = typename std::vector<T>::difference_type;
  using iterator = impl::UidVectorIterator<typename std::vector<T>::iterator>;
  using const_iterator =
      impl::UidVectorIterator<typename std::vector<T>::const_iterator>;

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
    if (uid >= m_vector.size() || !m_vector[uid]) {
      return;
    }
    m_free.push_back(uid);
    m_vector[uid] = T();
    --m_active_count;
  }

  // Removes all elements.
  void clear() {
    m_vector.clear();
    m_free.clear();
    m_active_count = 0;
  }

  // Iterator access
  iterator begin() noexcept {
    return iterator(m_vector.begin(), m_vector.end());
  }
  const_iterator begin() const noexcept {
    return const_iterator(m_vector.begin(), m_vector.end());
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(m_vector.begin(), m_vector.end());
  }
  iterator end() noexcept { return iterator(m_vector.end(), m_vector.end()); }
  const_iterator end() const noexcept {
    return const_iterator(m_vector.end(), m_vector.end());
  }
  const_iterator cend() const noexcept {
    return const_iterator(m_vector.end(), m_vector.end());
  }

 private:
  std::vector<T> m_vector;
  std::vector<size_type> m_free;
  size_type m_active_count{0};
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_UIDVECTOR_H_
