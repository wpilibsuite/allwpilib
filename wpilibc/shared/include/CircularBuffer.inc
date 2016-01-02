/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <algorithm>

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

template <class T>
void CircularBuffer<T>::Reset() {
  std::fill(m_data.begin(), m_data.end(), 0);
  m_front = 0;
  m_length = 0;
}

/**
 * Returns element at index starting from front of buffer.
 */
template <class T>
T& CircularBuffer<T>::operator[](size_t index) {
  return m_data[(m_front + index) % m_data.size()];
}

/**
 * Returns element at index starting from front of buffer.
 */
template <class T>
const T& CircularBuffer<T>::operator[](size_t index) const {
  return m_data[(m_front + index) % m_data.size()];
}

/**
 * Increment an index modulo the length of the m_data buffer
 */
template <class T>
size_t CircularBuffer<T>::ModuloInc(size_t index) {
  return (index + 1) % m_data.size();
}

/**
 * Decrement an index modulo the length of the m_data buffer
 */
template <class T>
size_t CircularBuffer<T>::ModuloDec(size_t index) {
  if (index == 0) {
    return m_data.size() - 1;
  } else {
    return index - 1;
  }
}
