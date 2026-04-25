// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace wpi::filterdesigner::detail {

// Internal representation for emitted code: one element per line, no
// embedded newlines. Newlines are added at the boundary by Join.
using Lines = std::vector<std::string>;

// Joins @a lines with '\n' separators and a trailing '\n', so the result
// round-trips through SplitLines.
inline std::string Join(const Lines& lines) {
  std::string out;
  for (const auto& line : lines) {
    out.append(line);
    out.push_back('\n');
  }
  return out;
}

// Splits @a body on '\n'. A trailing newline is consumed, so "A\nB\n" and
// "A\nB" both produce {"A", "B"}.
inline Lines SplitLines(std::string_view body) {
  Lines out;
  size_t pos = 0;
  while (true) {
    size_t nl = body.find('\n', pos);
    if (nl == std::string_view::npos) {
      if (pos < body.size()) {
        out.emplace_back(body.substr(pos));
      }
      return out;
    }
    out.emplace_back(body.substr(pos, nl - pos));
    pos = nl + 1;
  }
}

// Returns @a body with @a prefix prepended to every line.
inline Lines PrefixLines(const Lines& body, std::string_view prefix) {
  Lines out;
  out.reserve(body.size());
  for (const auto& line : body) {
    out.push_back(fmt::format("{}{}", prefix, line));
  }
  return out;
}

}  // namespace wpi::filterdesigner::detail
