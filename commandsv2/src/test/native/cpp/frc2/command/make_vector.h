// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>
#include <utility>
#include <vector>

namespace frc2 {

template <typename T = void, typename... Args,
          typename CommonType = std::conditional_t<
              std::same_as<T, void>, std::common_type_t<Args...>, T>>
  requires((std::is_constructible_v<T, Args> &&
            std::is_convertible_v<Args, T>) &&
           ...)
std::vector<CommonType> make_vector(Args&&... args) {
  if constexpr (std::is_trivially_copyable_v<CommonType>) {
    return std::vector<CommonType>{std::forward<Args>(args)...};
  } else {
    std::vector<CommonType> vec;
    vec.reserve(sizeof...(Args));

    using arr_t = int[];
    [[maybe_unused]]
    arr_t arr{0, (vec.emplace_back(std::forward<Args>(args)), 0)...};

    return vec;
  }
}

}  // namespace frc2
