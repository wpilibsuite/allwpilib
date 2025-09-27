// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace wpi {

// Binary insertion into vector; std::log(n) efficiency.
template <typename T>
typename std::vector<T>::iterator insert_sorted(std::vector<T>& vec,
                                                T const& item) {
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

/**
 * Calls f(i, elem) for each element of elems where i is the index of the
 * element in elems and elem is the element.
 *
 * @param f The callback.
 * @param elems The elements.
 */
template <typename F, typename... Ts>
constexpr void for_each(F&& f, Ts&&... elems) {
  [&]<size_t... Is>(std::index_sequence<Is...>) {
    (f(Is, elems), ...);
  }(std::index_sequence_for<Ts...>{});
}

}  // namespace wpi
