// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <vector>

namespace wpi {

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy
 * old values.
 */
template <class T>
class circular_buffer {
 public:
  explicit circular_buffer(size_t size);

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using size_type = size_t;
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;

  size_type size() const;
  T& front();
  const T& front() const;
  T& back();
  const T& back() const;
  void push_front(T value);
  void push_back(T value);
  T pop_front();
  T pop_back();
  void resize(size_t size);
  void reset();

  T& operator[](size_t index);
  const T& operator[](size_t index) const;

 private:
  std::vector<T> m_data;

  T zero_val{0};

  // Index of element at front of buffer
  size_t m_front = 0;

  // Number of elements used in buffer
  size_t m_length = 0;

  size_t ModuloInc(size_t index);
  size_t ModuloDec(size_t index);
};

}  // namespace wpi

#include "wpi/circular_buffer.inc"
