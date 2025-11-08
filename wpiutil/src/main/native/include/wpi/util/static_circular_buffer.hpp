// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <iterator>

namespace wpi {

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy
 * old values.
 *
 * @tparam T Buffer element type.
 * @tparam N Maximum number of buffer elements.
 */
template <class T, size_t N>
class static_circular_buffer {
 public:
  static_assert(N > 0, "Circular buffer size cannot be zero.");

  class iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr iterator(static_circular_buffer* buffer, size_t index)
        : m_buffer{buffer}, m_index{index} {}

    constexpr iterator& operator++() {
      ++m_index;
      return *this;
    }
    constexpr iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    constexpr iterator& operator--() {
      --m_index;
      return *this;
    }
    constexpr iterator operator--(int) {
      iterator retval = *this;
      --(*this);
      return retval;
    }
    constexpr bool operator==(const iterator&) const = default;
    constexpr reference operator*() { return (*m_buffer)[m_index]; }

   private:
    static_circular_buffer* m_buffer;
    size_t m_index;
  };

  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_reference = const T&;

    constexpr const_iterator(const static_circular_buffer* buffer, size_t index)
        : m_buffer{buffer}, m_index{index} {}

    constexpr const_iterator& operator++() {
      ++m_index;
      return *this;
    }
    constexpr const_iterator operator++(int) {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }
    constexpr const_iterator& operator--() {
      --m_index;
      return *this;
    }
    constexpr const_iterator operator--(int) {
      const_iterator retval = *this;
      --(*this);
      return retval;
    }
    constexpr bool operator==(const const_iterator&) const = default;
    constexpr const_reference operator*() const { return (*m_buffer)[m_index]; }

   private:
    const static_circular_buffer* m_buffer;
    size_t m_index;
  };

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /**
   * Returns begin iterator.
   */
  constexpr iterator begin() { return iterator(this, 0); }

  /**
   * Returns end iterator.
   */
  constexpr iterator end() {
    return iterator(this, ::wpi::static_circular_buffer<T, N>::size());
  }

  /**
   * Returns const begin iterator.
   */
  constexpr const_iterator begin() const { return const_iterator(this, 0); }

  /**
   * Returns const end iterator.
   */
  constexpr const_iterator end() const {
    return const_iterator(this, ::wpi::static_circular_buffer<T, N>::size());
  }

  /**
   * Returns const begin iterator.
   */
  constexpr const_iterator cbegin() const { return const_iterator(this, 0); }

  /**
   * Returns const end iterator.
   */
  constexpr const_iterator cend() const {
    return const_iterator(this, ::wpi::static_circular_buffer<T, N>::size());
  }

  /**
   * Returns reverse begin iterator.
   */
  constexpr reverse_iterator rbegin() { return reverse_iterator(end()); }

  /**
   * Returns reverse end iterator.
   */
  constexpr reverse_iterator rend() { return reverse_iterator(begin()); }

  /**
   * Returns const reverse begin iterator.
   */
  constexpr const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  /**
   * Returns const reverse end iterator.
   */
  constexpr const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  /**
   * Returns const reverse begin iterator.
   */
  constexpr const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  /**
   * Returns const reverse end iterator.
   */
  constexpr const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  /**
   * Returns number of elements in buffer
   */
  constexpr size_t size() const { return m_length; }

  /**
   * Returns value at front of buffer
   */
  constexpr T& front() { return (*this)[0]; }

  /**
   * Returns value at front of buffer
   */
  constexpr const T& front() const { return (*this)[0]; }

  /**
   * Returns value at back of buffer
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  constexpr T& back() { return m_data[(m_front + m_length - 1) % N]; }

  /**
   * Returns value at back of buffer
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  constexpr const T& back() const {
    return m_data[(m_front + m_length - 1) % N];
  }

  /**
   * Push a new value onto the front of the buffer.
   *
   * The value at the back is overwritten if the buffer is full.
   */
  constexpr void push_front(T value) {
    m_front = ModuloDec(m_front);

    m_data[m_front] = value;

    if (m_length < N) {
      m_length++;
    }
  }

  /**
   * Push a new value onto the back of the buffer.
   *
   * The value at the front is overwritten if the buffer is full.
   */
  constexpr void push_back(T value) {
    m_data[(m_front + m_length) % N] = value;

    if (m_length < N) {
      m_length++;
    } else {
      // Increment front if buffer is full to maintain size
      m_front = ModuloInc(m_front);
    }
  }

  /**
   * Push a new value onto the front of the buffer that is constructed with the
   * provided constructor arguments.
   *
   * The value at the back is overwritten if the buffer is full.
   */
  template <class... Args>
  constexpr void emplace_front(Args&&... args) {
    m_front = ModuloDec(m_front);

    m_data[m_front] = T{args...};

    if (m_length < N) {
      m_length++;
    }
  }

  /**
   * Push a new value onto the back of the buffer that is constructed with the
   * provided constructor arguments.
   *
   * The value at the front is overwritten if the buffer is full.
   */
  template <class... Args>
  constexpr void emplace_back(Args&&... args) {
    m_data[(m_front + m_length) % N] = T{args...};

    if (m_length < N) {
      m_length++;
    } else {
      // Increment front if buffer is full to maintain size
      m_front = ModuloInc(m_front);
    }
  }

  /**
   * Pop value at front of buffer.
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  constexpr T pop_front() {
    T& temp = m_data[m_front];
    m_front = ModuloInc(m_front);
    m_length--;
    return temp;
  }

  /**
   * Pop value at back of buffer.
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  constexpr T pop_back() {
    m_length--;
    return m_data[(m_front + m_length) % N];
  }

  /**
   * Empties internal buffer.
   */
  constexpr void reset() {
    m_front = 0;
    m_length = 0;
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  constexpr T& operator[](size_t index) {
    return m_data[(m_front + index) % N];
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  constexpr const T& operator[](size_t index) const {
    return m_data[(m_front + index) % N];
  }

 private:
  std::array<T, N> m_data;

  // Index of element at front of buffer
  size_t m_front = 0;

  // Number of elements used in buffer
  size_t m_length = 0;

  /**
   * Increment an index modulo the length of the buffer.
   *
   * @return The result of the modulo operation.
   */
  constexpr size_t ModuloInc(size_t index) { return (index + 1) % N; }

  /**
   * Decrement an index modulo the length of the buffer.
   *
   * @return The result of the modulo operation.
   */
  constexpr size_t ModuloDec(size_t index) {
    if (index == 0) {
      return N - 1;
    } else {
      return index - 1;
    }
  }
};

}  // namespace wpi
