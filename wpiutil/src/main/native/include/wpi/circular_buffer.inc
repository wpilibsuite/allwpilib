// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/circular_buffer.h"

namespace wpi {

/**
 * Resizes internal buffer to given size.
 */
template <class T>
void circular_buffer<T>::resize(size_t size) {
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

}  // namespace wpi
