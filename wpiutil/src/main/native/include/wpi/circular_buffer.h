// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <iterator>
#include <vector>

namespace wpi {

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy
 * old values.
 *
 * @tparam T Buffer element type.
 */
template <class T>
class circular_buffer {
 public:
  /**
   * Constructs a circular buffer.
   *
   * @param size Maximum number of buffer elements.
   */
  constexpr explicit circular_buffer(size_t size) : m_data(size, T{}) {}

  constexpr circular_buffer(const circular_buffer&) = default;
  constexpr circular_buffer& operator=(const circular_buffer&) = default;
  constexpr circular_buffer(circular_buffer&&) = default;
  constexpr circular_buffer& operator=(circular_buffer&&) = default;

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr iterator(circular_buffer* buffer, size_t index)
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
    constexpr bool operator==(const iterator&) const = default;
    constexpr reference operator*() { return (*m_buffer)[m_index]; }

   private:
    circular_buffer* m_buffer;
    size_t m_index;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_reference = const T&;

    constexpr const_iterator(const circular_buffer* buffer, size_t index)
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
    constexpr bool operator==(const const_iterator&) const = default;
    constexpr const_reference operator*() const { return (*m_buffer)[m_index]; }

   private:
    const circular_buffer* m_buffer;
    size_t m_index;
  };

  constexpr iterator begin() { return iterator(this, 0); }
  constexpr iterator end() {
    return iterator(this, ::wpi::circular_buffer<T>::size());
  }

  constexpr const_iterator begin() const { return const_iterator(this, 0); }
  constexpr const_iterator end() const {
    return const_iterator(this, ::wpi::circular_buffer<T>::size());
  }

  constexpr const_iterator cbegin() const { return const_iterator(this, 0); }
  constexpr const_iterator cend() const {
    return const_iterator(this, ::wpi::circular_buffer<T>::size());
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
  constexpr T& back() {
    return m_data[(m_front + m_length - 1) % m_data.size()];
  }

  /**
   * Returns value at back of buffer
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  constexpr const T& back() const {
    return m_data[(m_front + m_length - 1) % m_data.size()];
  }

  /**
   * Push a new value onto the front of the buffer.
   *
   * The value at the back is overwritten if the buffer is full.
   */
  constexpr void push_front(T value) {
    if (m_data.size() == 0) {
      return;
    }

    m_front = ModuloDec(m_front);

    m_data[m_front] = value;

    if (m_length < m_data.size()) {
      m_length++;
    }
  }

  /**
   * Push a new value onto the back of the buffer.
   *
   * The value at the front is overwritten if the buffer is full.
   */
  constexpr void push_back(T value) {
    if (m_data.size() == 0) {
      return;
    }

    m_data[(m_front + m_length) % m_data.size()] = value;

    if (m_length < m_data.size()) {
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
    if (m_data.size() == 0) {
      return;
    }

    m_front = ModuloDec(m_front);

    m_data[m_front] = T{args...};

    if (m_length < m_data.size()) {
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
    if (m_data.size() == 0) {
      return;
    }

    m_data[(m_front + m_length) % m_data.size()] = T{args...};

    if (m_length < m_data.size()) {
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
    return m_data[(m_front + m_length) % m_data.size()];
  }

  /**
   * Resizes internal buffer to given size.
   */
  constexpr void resize(size_t size) {
    if (size > m_data.size()) {
      // Find end of buffer
      size_t insertLocation = (m_front + m_length) % m_data.size();

      // If insertion location precedes front of buffer, push front index back
      if (insertLocation <= m_front) {
        m_front += size - m_data.size();
      }

      // Add elements to end of buffer
      m_data.insert(m_data.begin() + insertLocation, size - m_data.size(), 0);
    } else if (size < m_data.size()) {
      /* 1) Shift element block start at "front" left as many blocks as were
       *    removed up to but not exceeding buffer[0]
       * 2) Shrink buffer, which will remove even more elements automatically if
       *    necessary
       */
      size_t elemsToRemove = m_data.size() - size;
      auto frontIter = m_data.begin() + m_front;
      if (m_front < elemsToRemove) {
        /* Remove elements from end of buffer before shifting start of element
         * block. Doing so saves a few copies.
         */
        m_data.erase(frontIter + size, m_data.end());

        // Shift start of element block to left
        m_data.erase(m_data.begin(), frontIter);

        // Update metadata
        m_front = 0;
      } else {
        // Shift start of element block to left
        m_data.erase(frontIter - elemsToRemove, frontIter);

        // Update metadata
        m_front -= elemsToRemove;
      }

      /* Length only changes during a shrink if all unused spaces have been
       * removed. Length decreases as used spaces are removed to meet the
       * required size.
       */
      if (m_length > size) {
        m_length = size;
      }
    }
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
    return m_data[(m_front + index) % m_data.size()];
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  constexpr const T& operator[](size_t index) const {
    return m_data[(m_front + index) % m_data.size()];
  }

 private:
  std::vector<T> m_data;

  // Index of element at front of buffer
  size_t m_front = 0;

  // Number of elements used in buffer
  size_t m_length = 0;

  /**
   * Increment an index modulo the size of the buffer.
   *
   * @param index Index into the buffer.
   * @return The incremented index.
   */
  constexpr size_t ModuloInc(size_t index) {
    return (index + 1) % m_data.size();
  }

  /**
   * Decrement an index modulo the size of the buffer.
   *
   * @param index Index into the buffer.
   * @return The decremented index.
   */
  constexpr size_t ModuloDec(size_t index) {
    if (index == 0) {
      return m_data.size() - 1;
    } else {
      return index - 1;
    }
  }
};

}  // namespace wpi
