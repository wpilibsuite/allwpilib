/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>

namespace frc {

template <class T>
CircularBuffer<T>::CircularBuffer(size_t size) : m_data(size, 0) {}

/**
 * Push new value onto front of the buffer. The value at the back is overwritten
 * if the buffer is full.
 */
template <class T>
void CircularBuffer<T>::PushFront(T value) {
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
 * Push new value onto back of the buffer. The value at the front is overwritten
 * if the buffer is full.
 */
template <class T>
void CircularBuffer<T>::PushBack(T value) {
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
 * Pop value at front of buffer.
 */
template <class T>
T CircularBuffer<T>::PopFront() {
  // If there are no elements in the buffer, do nothing
  if (m_length == 0) {
    return 0;
  }

  T& temp = m_data[m_front];
  m_front = ModuloInc(m_front);
  m_length--;
  return temp;
}

/**
 * Pop value at back of buffer.
 */
template <class T>
T CircularBuffer<T>::PopBack() {
  // If there are no elements in the buffer, do nothing
  if (m_length == 0) {
    return 0;
  }

  m_length--;
  return m_data[(m_front + m_length) % m_data.size()];
}

/**
 * Resizes internal buffer to given size.
 */
template <class T>
void CircularBuffer<T>::Resize(size_t size) {
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
 * Sets internal buffer contents to zero.
 */
template <class T>
void CircularBuffer<T>::Reset() {
  std::fill(m_data.begin(), m_data.end(), 0);
  m_front = 0;
  m_length = 0;
}

/**
 * @return Element at index starting from front of buffer.
 */
template <class T>
T& CircularBuffer<T>::operator[](size_t index) {
  return m_data[(m_front + index) % m_data.size()];
}

/**
 * @return Element at index starting from front of buffer.
 */
template <class T>
const T& CircularBuffer<T>::operator[](size_t index) const {
  return m_data[(m_front + index) % m_data.size()];
}

/**
 * Increment an index modulo the length of the buffer.
 *
 * @return The result of the modulo operation.
 */
template <class T>
size_t CircularBuffer<T>::ModuloInc(size_t index) {
  return (index + 1) % m_data.size();
}

/**
 * Decrement an index modulo the length of the buffer.
 *
 * @return The result of the modulo operation.
 */
template <class T>
size_t CircularBuffer<T>::ModuloDec(size_t index) {
  if (index == 0) {
    return m_data.size() - 1;
  } else {
    return index - 1;
  }
}

}  // namespace frc
