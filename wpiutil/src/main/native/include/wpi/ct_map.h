// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace wpi {

template <typename Key, typename Value, size_t Size>
class ct_map {
  constexpr ct_map();

  [[nodiscard]]
  constexpr const Value& operator[](const Key& key) const {
    if (const auto it =
            std::find_if(data.begin(), data.end(),
                         [&key](const auto& v) { return v.first == key; });
        it != data.end()) {
      return it->second;
    } else {
      throw std::range_error("Not found");
    }
  }

 private:
  std::array<std::pair<Key, Value>, Size> data;
};

}  // namespace wpi
