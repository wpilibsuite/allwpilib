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

#include "wpi/util/StringExtras.hpp"

#include <algorithm>
#include <cstdlib>
#include <string_view>

#include "wpi/util/SmallString.hpp"

// strncasecmp() is not available on non-POSIX systems, so define an
// alternative function here.
static int ascii_strncasecmp(const char* lhs, const char* rhs,
                             size_t length) noexcept {
  for (size_t i = 0; i < length; ++i) {
    unsigned char lhc = wpi::util::toLower(lhs[i]);
    unsigned char rhc = wpi::util::toLower(rhs[i]);
    if (lhc != rhc) {
      return lhc < rhc ? -1 : 1;
    }
  }
  return 0;
}

int wpi::util::compare_lower(std::string_view lhs, std::string_view rhs) noexcept {
  if (int Res = ascii_strncasecmp(lhs.data(), rhs.data(),
                                  (std::min)(lhs.size(), rhs.size()))) {
    return Res;
  }
  if (lhs.size() == rhs.size()) {
    return 0;
  }
  return lhs.size() < rhs.size() ? -1 : 1;
}

std::string_view::size_type wpi::util::find_lower(
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

std::string_view::size_type wpi::util::find_lower(
    std::string_view str, std::string_view other,
    std::string_view::size_type from) noexcept {
  auto s = substr(str, from);
  while (s.size() >= other.size()) {
    if (starts_with_lower(s, other)) {
      return from;
    }
    s.remove_prefix(1);
    ++from;
  }
  return std::string_view::npos;
}

std::string_view::size_type wpi::util::rfind_lower(
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

std::string_view::size_type wpi::util::rfind_lower(std::string_view str,
                                             std::string_view other) noexcept {
  std::string_view::size_type n = other.size();
  if (n > str.size()) {
    return std::string_view::npos;
  }
  for (size_t i = str.size() - n + 1, e = 0; i != e;) {
    --i;
    if (equals_lower(substr(str, i, n), other)) {
      return i;
    }
  }
  return std::string_view::npos;
}

bool wpi::util::starts_with_lower(std::string_view str,
                            std::string_view prefix) noexcept {
  return str.size() >= prefix.size() &&
         ascii_strncasecmp(str.data(), prefix.data(), prefix.size()) == 0;
}

bool wpi::util::ends_with_lower(std::string_view str,
                          std::string_view suffix) noexcept {
  return str.size() >= suffix.size() &&
         ascii_strncasecmp(str.data() + str.size() - suffix.size(),
                           suffix.data(), suffix.size()) == 0;
}

template <>
std::optional<float> wpi::util::parse_float<float>(std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::util::SmallString<32> storage{str};
  char* end;
  float val = std::strtof(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}

template <>
std::optional<double> wpi::util::parse_float<double>(std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::util::SmallString<32> storage{str};
  char* end;
  double val = std::strtod(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}

template <>
std::optional<long double> wpi::util::parse_float<long double>(
    std::string_view str) noexcept {
  if (str.empty()) {
    return std::nullopt;
  }
  wpi::util::SmallString<32> storage{str};
  char* end;
  long double val = std::strtold(storage.c_str(), &end);
  if (*end != '\0') {
    return std::nullopt;
  }
  return val;
}

std::pair<std::string_view, std::string_view> wpi::util::UnescapeCString(
    std::string_view str, wpi::util::SmallVectorImpl<char>& buf) {
  buf.clear();
  buf.reserve(str.size());
  const char* s = str.data();
  const char* end = str.data() + str.size();
  for (; s != end && *s != '"'; ++s) {
    if (*s != '\\' || (s + 1) >= end) {
      buf.push_back(*s);
      continue;
    }
    switch (*++s) {
      case 'a':
        buf.push_back('\a');
        break;
      case 'b':
        buf.push_back('\b');
        break;
      case 'f':
        buf.push_back('\f');
        break;
      case 'n':
        buf.push_back('\n');
        break;
      case 'r':
        buf.push_back('\r');
        break;
      case 't':
        buf.push_back('\t');
        break;
      case 'v':
        buf.push_back('\v');
        break;
      case 'x': {
        // hex escape
        if ((s + 1) >= end || !isxdigit(*(s + 1))) {
          buf.push_back('x');  // treat it like a unknown escape
          break;
        }
        unsigned int ch = wpi::util::hexDigitValue(*++s);
        if ((s + 1) < end && std::isxdigit(*(s + 1))) {
          ch <<= 4;
          ch |= wpi::util::hexDigitValue(*++s);
        }
        buf.push_back(static_cast<char>(ch));
        break;
      }
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        // octal escape
        unsigned int ch = *s - '0';
        if ((s + 1) < end && wpi::util::isDigit(*(s + 1))) {
          ch <<= 3;
          ch |= *++s - '0';
        }
        if ((s + 1) < end && wpi::util::isDigit(*(s + 1))) {
          ch <<= 3;
          ch |= *++s - '0';
        }
        buf.push_back(static_cast<char>(ch));
        break;
      }
      default:
        buf.push_back(*s);
        break;
    }
  }
  if (s == end) {
    return {{buf.data(), buf.size()}, {}};
  } else {
    return {{buf.data(), buf.size()}, {s, static_cast<size_t>(end - s)}};
  }
}
