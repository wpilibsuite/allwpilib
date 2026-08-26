// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>

#include <catch2/catch_tostring.hpp>

#include "wpi/util/json.hpp"

namespace Catch {

template <typename T>
struct StringMaker<std::span<T>> {
  static std::string convert(std::span<T> val) {
    std::string rv = "{";
    bool first = true;
    for (auto v : val) {
      if (first) {
        first = false;
      } else {
        rv += ", ";
      }
      rv += Detail::stringify(v);
    }
    rv += '}';
    return rv;
  }
};

template <>
struct StringMaker<wpi::json> {
  static std::string convert(const wpi::json& val) { return val.to_string(); }
};

}  // namespace Catch
