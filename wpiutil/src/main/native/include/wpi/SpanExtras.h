// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cassert>

#include "wpi/span.h"

namespace wpi {

/// Drop the first \p N elements of the array.
template <typename T>
constexpr span<T> drop_front(span<T> in, typename span<T>::size_type n = 1) {
  assert(in.size() >= n && "Dropping more elements than exist");
  return in.subspan(n, in.size() - n);
}

/// Drop the last \p N elements of the array.
template <typename T>
constexpr span<T> drop_back(span<T> in, typename span<T>::size_type n = 1) {
  assert(in.size() >= n && "Dropping more elements than exist");
  return in.subspan(0, in.size() - n);
}

}  // namespace wpi
