// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <ostream>
#include <span>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "wpi/json.h"

namespace wpi {

inline void PrintTo(std::string_view str, ::std::ostream* os) {
  ::testing::internal::PrintStringTo(std::string{str}, os);
}

template <typename T>
void PrintTo(std::span<T> val, ::std::ostream* os) {
  *os << '{';
  bool first = true;
  for (auto v : val) {
    if (first) {
      first = false;
    } else {
      *os << ", ";
    }
    *os << ::testing::PrintToString(v);
  }
  *os << '}';
}

inline void PrintTo(const json& val, ::std::ostream* os) {
  *os << val.dump();
}

}  // namespace wpi
