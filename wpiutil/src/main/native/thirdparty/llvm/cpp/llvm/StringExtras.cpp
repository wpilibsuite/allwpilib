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
    if (equals_lower(substr(str, i, n), other)) {
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

static unsigned GetAutoSenseRadix(std::string_view& str) noexcept {
  if (str.empty()) {
    return 10;
  }

  if (wpi::starts_with(str, "0x") || wpi::starts_with(str, "0X")) {
    str.remove_prefix(2);
    return 16;
  }

  if (wpi::starts_with(str, "0b") || wpi::starts_with(str, "0B")) {
    str.remove_prefix(2);
    return 2;
  }

  if (wpi::starts_with(str, "0o")) {
    str.remove_prefix(2);
    return 8;
  }

  if (str[0] == '0' && str.size() > 1 && wpi::isDigit(str[1])) {
    str.remove_prefix(1);
    return 8;
  }

  return 10;
}

bool wpi::detail::ConsumeUnsignedInteger(
    std::string_view& str, unsigned radix,
    unsigned long long& result) noexcept {  // NOLINT(runtime/int)
  // Autosense radix if not specified.
  if (radix == 0) {
    radix = GetAutoSenseRadix(str);
  }

  // Empty strings (after the radix autosense) are invalid.
  if (str.empty()) {
    return true;
  }

  // Parse all the bytes of the string given this radix.  Watch for overflow.
  std::string_view str2 = str;
  result = 0;
  while (!str2.empty()) {
    unsigned charVal;
    if (str2[0] >= '0' && str2[0] <= '9') {
      charVal = str2[0] - '0';
    } else if (str2[0] >= 'a' && str2[0] <= 'z') {
      charVal = str2[0] - 'a' + 10;
    } else if (str2[0] >= 'A' && str2[0] <= 'Z') {
      charVal = str2[0] - 'A' + 10;
    } else {
      break;
    }

    // If the parsed value is larger than the integer radix, we cannot
    // consume any more characters.
    if (charVal >= radix) {
      break;
    }

    // Add in this character.
    unsigned long long prevResult = result;  // NOLINT(runtime/int)
    result = result * radix + charVal;

    // Check for overflow by shifting back and seeing if bits were lost.
    if (result / radix < prevResult) {
      return true;
    }

    str2.remove_prefix(1);
  }

  // We consider the operation a failure if no characters were consumed
  // successfully.
  if (str.size() == str2.size()) {
    return true;
  }

  str = str2;
  return false;
}

bool wpi::detail::ConsumeSignedInteger(
    std::string_view& str, unsigned radix,
    long long& result) noexcept {  // NOLINT(runtime/int)
  unsigned long long ullVal;       // NOLINT(runtime/int)

  // Handle positive strings first.
  if (str.empty() || str.front() != '-') {
    if (wpi::detail::ConsumeUnsignedInteger(str, radix, ullVal) ||
        // Check for value so large it overflows a signed value.
        static_cast<long long>(ullVal) < 0) {  // NOLINT(runtime/int)
      return true;
    }
    result = ullVal;
    return false;
  }

  // Get the positive part of the value.
  std::string_view str2 = wpi::drop_front(str);
  if (wpi::detail::ConsumeUnsignedInteger(str2, radix, ullVal) ||
      // Reject values so large they'd overflow as negative signed, but allow
      // "-0".  This negates the unsigned so that the negative isn't undefined
      // on signed overflow.
      static_cast<long long>(-ullVal) > 0) {  // NOLINT(runtime/int)
    return true;
  }

  str = str2;
  result = -ullVal;
  return false;
}

bool wpi::detail::GetAsUnsignedInteger(
    std::string_view str, unsigned radix,
    unsigned long long& result) noexcept {  // NOLINT(runtime/int)
  if (wpi::detail::ConsumeUnsignedInteger(str, radix, result)) {
    return true;
  }

  // For getAsUnsignedInteger, we require the whole string to be consumed or
  // else we consider it a failure.
  return !str.empty();
}

bool wpi::detail::GetAsSignedInteger(
    std::string_view str, unsigned radix,
    long long& result) noexcept {  // NOLINT(runtime/int)
  if (wpi::detail::ConsumeSignedInteger(str, radix, result)) {
    return true;
  }

  // For getAsSignedInteger, we require the whole string to be consumed or else
  // we consider it a failure.
  return !str.empty();
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

std::pair<std::string_view, std::string_view> wpi::UnescapeCString(
    std::string_view str, wpi::SmallVectorImpl<char>& buf) {
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
        unsigned int ch = wpi::hexDigitValue(*++s);
        if ((s + 1) < end && std::isxdigit(*(s + 1))) {
          ch <<= 4;
          ch |= wpi::hexDigitValue(*++s);
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
        if ((s + 1) < end && wpi::isDigit(*(s + 1))) {
          ch <<= 3;
          ch |= *++s - '0';
        }
        if ((s + 1) < end && wpi::isDigit(*(s + 1))) {
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
