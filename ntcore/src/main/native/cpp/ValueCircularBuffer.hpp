// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include "wpi/util/circular_buffer.hpp"

#include "Value_internal.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_cpp_types.hpp"

namespace nt {

class ValueCircularBuffer {
 public:
  explicit ValueCircularBuffer(size_t size) : m_storage{size} {}

  template <class... Args>
  void emplace_back(Args&&... args) {
    m_storage.emplace_back(std::forward<Args...>(args...));
  }

  std::vector<Value> ReadValue(unsigned int types);
  template <ValidType T>
  std::vector<Timestamped<typename TypeInfo<T>::Value>> Read();

 private:
  wpi::circular_buffer<Value> m_storage;
};

template <ValidType T>
std::vector<Timestamped<typename TypeInfo<T>::Value>>
ValueCircularBuffer::Read() {
  std::vector<Timestamped<typename TypeInfo<T>::Value>> rv;
  rv.reserve(m_storage.size());
  for (auto&& val : m_storage) {
    if (IsNumericConvertibleTo<T>(val) || IsType<T>(val)) {
      rv.emplace_back(GetTimestamped<T, true>(val));
    }
  }
  m_storage.reset();
  return rv;
}

}  // namespace nt
