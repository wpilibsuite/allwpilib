// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===-- StringExtras.cpp - Implement the StringExtras header --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the StringExtras.h header
//
//===----------------------------------------------------------------------===//

#include "wpi/StringExtras.h"

#include <algorithm>
#include <cstdlib>
#include <string_view>

#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"

// strncasecmp() is not available on non-POSIX systems, so define an
// alternative function here.
static int ascii_strncasecmp(const char* lhs, const char* rhs,
                             size_t length) noexcept {
  for (size_t i = 0; i < length; ++i) {
    unsigned char lhc = wpi::toLower(lhs[i]);
    unsigned char rhc = wpi::toLower(rhs[i]);
    if (lhc != rhc) {
      return lhc < rhc ? -1 : 1;
    }
  }
  return 0;
}

int wpi::compare_lower(std::string_view lhs, std::string_view rhs) noexcept {
  if (int Res = ascii_strncasecmp(lhs.data(), rhs.data(),
                                  (std::min)(lhs.size(), rhs.size()))) {
    return Res;
  }
  if (lhs.size() == rhs.size()) {
    return 0;
  }
  return lhs.size() < rhs.size() ? -1 : 1;
}

std::string_view::size_type wpi::find_lower(
    std::string_view str, char ch, std::string_view::size_type from) noexcept {
  char lch = toLower(ch);
  auto s = drop_front(str, from);
  while (!s.empty()) {
    if (toLower(s.front()) == lch) {
      return str.size() - s.size();
    }
    s.remove_prefix(1);
  }
  return std::string_view::npos;
}

std::string_view::size_type wpi::find_lower(
    std::string_view str, std::string_view other,
    std::string_view::size_type from) noexcept {
  auto s = str.substr(from);
  while (s.size() >= other.size()) {
    if (starts_with_lower(s, other)) {
      return from;
    }
    s.remove_prefix(1);
    ++from;
  }
  return std::string_view::npos;
}

std::string_view::size_type wpi::rfind_lower(
    std::string_view str, char ch, std::string_view::size_type from) noexcept {
  from = (std::min)(from, str.size());
  auto data = str.data();
  std::string_view::size_type i = from;
  while (i != 0) {
    --i;
    if (toLower(data[i]) == toLower(ch)) {
      return i;
    }
  }
  return std::string_view::npos;
}

std::string_view::size_type wpi::rfind_lower(std::string_view str,
                                             std::string_view other) noexcept {
  std::string_view::size_type n = other.size();
  if (n > str.size()) {
    return std::string_view::npos;
  }
  for (size_t i = str.size() - n + 1, e = 0; i != e;) {
    --i;
    if (equals_lower(str.substr(i, n), other)) {
      return i;
    }
  }
  return std::string_view::npos;
}

bool wpi::starts_with_lower(std::string_view str,
                            std::string_view prefix) noexcept {
  return str.size() >= prefix.size() &&
         ascii_strncasecmp(str.data(), prefix.data(), prefix.size()) == 0;
}

bool wpi::ends_with_lower(std::string_view str,
                          std::string_view suffix) noexcept {
  return str.size() >= suffix.size() &&
         ascii_strncasecmp(str.data() + str.size() - suffix.size(),
                           suffix.data(), suffix.size()) == 0;
}

void wpi::split(std::string_view str, SmallVectorImpl<std::string_view>& arr,
                std::string_view separator, int maxSplit,
                bool keepEmpty) noexcept {
  std::string_view s = str;

  // Count down from maxSplit. When maxSplit is -1, this will just split
  // "forever". This doesn't support splitting more than 2^31 times
  // intentionally; if we ever want that we can make maxSplit a 64-bit integer
  // but that seems unlikely to be useful.
  while (maxSplit-- != 0) {
    auto idx = s.find(separator);
    if (idx == std::string_view::npos) {
      break;
    }

    // Push this split.
    if (keepEmpty || idx > 0) {
      arr.push_back(slice(s, 0, idx));
    }

    // Jump forward.
    s = slice(s, idx + separator.size(), std::string_view::npos);
  }

  // Push the tail.
  if (keepEmpty || !s.empty()) {
    arr.push_back(s);
  }
}

void wpi::split(std::string_view str, SmallVectorImpl<std::string_view>& arr,
                char separator, int maxSplit, bool keepEmpty) noexcept {
  std::string_view s = str;

  // Count down from maxSplit. When maxSplit is -1, this will just split
  // "forever". This doesn't support splitting more than 2^31 times
  // intentionally; if we ever want that we can make maxSplit a 64-bit integer
  // but that seems unlikely to be useful.
  while (maxSplit-- != 0) {
    size_t idx = s.find(separator);
    if (idx == std::string_view::npos) {
      break;
    }

    // Push this split.
    if (keepEmpty || idx > 0) {
      arr.push_back(slice(s, 0, idx));
    }

    // Jump forward.
    s = slice(s, idx + 1, std::string_view::npos);
  }

  // Push the tail.
  if (keepEmpty || !s.empty()) {
    arr.push_back(s);
  }
}

bool wpi::detail::GetAsUnsignedInteger(
    std::string_view str, unsigned radix,
    unsigned long long& result) noexcept {  // NOLINT(runtime/int)
  return wpi::getAsUnsignedInteger(str, radix, result);
}

bool wpi::detail::GetAsSignedInteger(
    std::string_view str, unsigned radix,
    long long& result) noexcept {  // NOLINT(runtime/int)
  return wpi::getAsSignedInteger(str, radix, result);
}

bool wpi::detail::ConsumeUnsignedInteger(
    std::string_view& str, unsigned radix,
    unsigned long long& result) noexcept {  // NOLINT(runtime/int)
  wpi::StringRef sref = str;
  bool rv = wpi::consumeUnsignedInteger(sref, radix, result);
  str = sref;
  return rv;
}

bool wpi::detail::ConsumeSignedInteger(
    std::string_view& str, unsigned radix,
    long long& result) noexcept {  // NOLINT(runtime/int)
  wpi::StringRef sref = str;
  bool rv = wpi::consumeSignedInteger(sref, radix, result);
  str = sref;
  return rv;
}

template <>
std::optional<float> wpi::parse_float<float>(std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::SmallString<32> storage{str};
  char* end;
  float val = std::strtof(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}

template <>
std::optional<double> wpi::parse_float<double>(std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::SmallString<32> storage{str};
  char* end;
  double val = std::strtod(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}

template <>
std::optional<long double> wpi::parse_float<long double>(
    std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::SmallString<32> storage{str};
  char* end;
  long double val = std::strtold(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}
