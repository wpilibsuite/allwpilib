// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstring>
#include <string_view>

#include "wpi/util/string.h"

namespace wpi::util {

/** Converts a WPI_String to a string_view */
constexpr std::string_view to_string_view(const struct WPI_String* str) {
  if (str) {
    return {str->str, str->len};
  } else {
    return "";
  }
}

/** Converts a string_view to a WPI_String */
constexpr WPI_String make_string(std::string_view view) {
  return WPI_String{view.data(), view.size()};
}

/** Allocates a copy of a string_view and stores the result into a WPI_String */
inline WPI_String alloc_wpi_string(std::string_view view) {
  WPI_String out;
  size_t len = view.size();
  std::memcpy(WPI_AllocateString(&out, len), view.data(), len);
  return out;
}

/** Allocates a copy of a WPI_String */
inline WPI_String copy_wpi_string(const WPI_String& str) {
  if (str.str == nullptr || str.len == 0) {
    return WPI_String{nullptr, 0};
  }
  return alloc_wpi_string(to_string_view(&str));
}

}  // namespace wpi::util
