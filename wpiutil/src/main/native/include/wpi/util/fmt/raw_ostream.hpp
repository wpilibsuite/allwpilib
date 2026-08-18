// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <format>
#include <iterator>
#include <vector>

#include "wpi/util/raw_ostream.hpp"

namespace wpi::util {

inline void vprint(wpi::util::raw_ostream& os, std::string_view format_str,
                   std::format_args args) {
  std::vector<char> buffer;
  std::vformat_to(std::back_inserter(buffer), format_str, args);
  os.write(buffer.data(), buffer.size());
}

/**
 * Prints formatted data to the stream *os*.
 */
template <typename S, typename... Args>
void print(wpi::util::raw_ostream& os, const S& format_str, Args&&... args) {
  vprint(os, format_str, std::make_format_args(args...));
}

}  // namespace wpi::util
