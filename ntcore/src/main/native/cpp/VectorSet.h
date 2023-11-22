// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

namespace nt {

// Utility wrapper for making a set-like vector
template <typename T>
class VectorSet : public std::vector<T> {
 public:
  using iterator = typename std::vector<T>::iterator;
  void Add(T value) { this->push_back(value); }
  // returns true if element was present
  bool Remove(T value) { return std::erase(*this, value) != 0; }
};

}  // namespace nt
