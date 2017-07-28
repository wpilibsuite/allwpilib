/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cstddef>
#include <vector>

namespace frc {

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy
 * old values.
 */
template <class T>
class CircularBuffer {
 public:
  explicit CircularBuffer(size_t size);

  void PushFront(T value);
  void PushBack(T value);
  T PopFront();
  T PopBack();
  void Resize(size_t size);
  void Reset();

  T& operator[](size_t index);
  const T& operator[](size_t index) const;

 private:
  std::vector<T> m_data;

  // Index of element at front of buffer
  size_t m_front = 0;

  // Number of elements used in buffer
  size_t m_length = 0;

  size_t ModuloInc(size_t index);
  size_t ModuloDec(size_t index);
};

}  // namespace frc

#include "CircularBuffer.inc"
