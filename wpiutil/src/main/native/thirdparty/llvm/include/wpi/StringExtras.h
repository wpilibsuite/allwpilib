// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===- llvm/ADT/StringExtras.h - Useful string functions --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains some functions that are useful when dealing with strings.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace wpi {

template <typename T>
class SmallVectorImpl;

/// hexdigit - Return the hexadecimal character for the
/// given number \p X (which should be less than 16).
constexpr char hexdigit(unsigned X, bool LowerCase = false) noexcept {
  const char HexChar = LowerCase ? 'a' : 'A';
  return X < 10 ? '0' + X : HexChar + X - 10;
}

/// Interpret the given character \p C as a hexadecimal digit and return its
/// value.
///
/// If \p C is not a valid hex digit, -1U is returned.
constexpr unsigned hexDigitValue(char C) noexcept {
  if (C >= '0' && C <= '9') {
    return C - '0';
  }
  if (C >= 'a' && C <= 'f') {
    return C - 'a' + 10U;
  }
  if (C >= 'A' && C <= 'F') {
    return C - 'A' + 10U;
  }
  return (std::numeric_limits<unsigned>::max)();
}

/// Checks if character \p C is one of the 10 decimal digits.
constexpr bool isDigit(char C) noexcept {
  return C >= '0' && C <= '9';
}

/// Checks if character \p C is a hexadecimal numeric character.
constexpr bool isHexDigit(char C) noexcept {
  return hexDigitValue(C) != (std::numeric_limits<unsigned>::max)();
}

/// Checks if character \p C is a valid letter as classified by "C" locale.
constexpr bool isAlpha(char C) noexcept {
  return ('a' <= C && C <= 'z') || ('A' <= C && C <= 'Z');
}

/// Checks whether character \p C is either a decimal digit or an uppercase or
/// lowercase letter as classified by "C" locale.
constexpr bool isAlnum(char C) noexcept {
  return isAlpha(C) || isDigit(C);
}

/// Checks whether character \p C is valid ASCII (high bit is zero).
constexpr bool isASCII(char C) noexcept {
  return static_cast<unsigned char>(C) <= 127;
}

/// Checks whether character \p C is printable.
///
/// Locale-independent version of the C standard library isprint whose results
/// may differ on different platforms.
constexpr bool isPrint(char C) noexcept {
  unsigned char UC = static_cast<unsigned char>(C);
  return (0x20 <= UC) && (UC <= 0x7E);
}

/// Returns the corresponding lowercase character if \p x is uppercase.
constexpr char toLower(char x) noexcept {
  if (x >= 'A' && x <= 'Z') {
    return x - 'A' + 'a';
  }
  return x;
}

/// Returns the corresponding uppercase character if \p x is lowercase.
constexpr char toUpper(char x) noexcept {
  if (x >= 'a' && x <= 'z') {
    return x - 'a' + 'A';
  }
  return x;
}

inline std::string utohexstr(unsigned long long val,  // NOLINT(runtime/int)
                             bool lowerCase = false) {
  char buf[17];
  char* bufptr = std::end(buf);

  if (val == 0) {
    *--bufptr = '0';
  }

  while (val) {
    unsigned char mod = static_cast<unsigned char>(val) & 15;
    *--bufptr = hexdigit(mod, lowerCase);
    val >>= 4;
  }

  return std::string(bufptr, std::end(buf));
}

/**
 * equals - Check for string equality, this is more efficient than
 * compare() when the relative ordering of inequal strings isn't needed.
 */
constexpr bool equals(std::string_view lhs, std::string_view rhs) noexcept {
  auto length = lhs.size();
  return length == rhs.size() && std::string_view::traits_type::compare(
                                     lhs.data(), rhs.data(), length) == 0;
}

/**
 * compare_lower - Compare two strings, ignoring case.
 */
int compare_lower(std::string_view lhs, std::string_view rhs) noexcept;

/**
 * equals_lower - Check for string equality, ignoring case.
 */
constexpr bool equals_lower(std::string_view lhs,
                            std::string_view rhs) noexcept {
  return lhs.size() == rhs.size() && compare_lower(lhs, rhs) == 0;
}

/**
 * Search for the first character @p ch in @p str, ignoring case.
 *
 * @returns The index of the first occurrence of @p ch, or npos if not
 * found.
 */
std::string_view::size_type find_lower(
    std::string_view str, char ch,
    std::string_view::size_type from = 0) noexcept;

/**
 * Search for the first string @p other in @p str, ignoring case.
 *
 * @returns The index of the first occurrence of @p other, or npos if not
 * found.
 */
std::string_view::size_type find_lower(
    std::string_view str, std::string_view other,
    std::string_view::size_type from = 0) noexcept;

/**
 * Search for the first string @p other in @p str, ignoring case.
 *
 * @returns The index of the first occurrence of @p other, or npos if not
 * found.
 */
inline std::string_view::size_type find_lower(
    std::string_view str, const char* other,
    std::string_view::size_type from = 0) noexcept {
  return find_lower(str, std::string_view{other}, from);
}

/**
 * Search for the last character @p ch in @p str, ignoring case.
 *
 * @returns The index of the last occurrence of @p ch, or npos if not
 * found.
 */
std::string_view::size_type rfind_lower(
    std::string_view str, char ch,
    std::string_view::size_type from = std::string_view::npos) noexcept;

/**
 * Search for the last string @p other in @p str, ignoring case.
 *
 * @returns The index of the last occurrence of @p other, or npos if not
 * found.
 */
std::string_view::size_type rfind_lower(std::string_view str,
                                        std::string_view other) noexcept;

/**
 * Search for the last string @p other in @p str, ignoring case.
 *
 * @returns The index of the last occurrence of @p other, or npos if not
 * found.
 */
inline std::string_view::size_type rfind_lower(std::string_view str,
                                               const char* other) noexcept {
  return rfind_lower(str, std::string_view{other});
}

/**
 * Returns the substring of @p str from [start, start + n).
 *
 * @param start The index of the starting character in the substring; if
 * the index is npos or greater than the length of the string then the
 * empty substring will be returned.
 *
 * @param n The number of characters to included in the substring. If n
 * exceeds the number of characters remaining in the string, the string
 * suffix (starting with @p start) will be returned.
 */
constexpr std::string_view substr(
    std::string_view str, std::string_view::size_type start,
    std::string_view::size_type n = std::string_view::npos) noexcept {
  auto length = str.size();
  start = (std::min)(start, length);
  return {str.data() + start, (std::min)(n, length - start)};
}

/**
 * Checks if @p str starts with the given @p prefix.
 */
constexpr bool starts_with(std::string_view str,
                           std::string_view prefix) noexcept {
  return substr(str, 0, prefix.size()) == prefix;
}

/**
 * Checks if @p str starts with the given @p prefix.
 */
constexpr bool starts_with(std::string_view str, char prefix) noexcept {
  return !str.empty() && std::string_view::traits_type::eq(str.front(), prefix);
}

/**
 * Checks if @p str starts with the given @p prefix.
 */
constexpr bool starts_with(std::string_view str, const char* prefix) noexcept {
  return starts_with(str, std::string_view(prefix));
}

/**
 * Checks if @p str starts with the given @p prefix, ignoring case.
 */
bool starts_with_lower(std::string_view str, std::string_view prefix) noexcept;

/**
 * Checks if @p str starts with the given @p prefix, ignoring case.
 */
constexpr bool starts_with_lower(std::string_view str, char prefix) noexcept {
  return !str.empty() && toLower(str.front()) == toLower(prefix);
}

/**
 * Checks if @p str starts with the given @p prefix, ignoring case.
 */
inline bool starts_with_lower(std::string_view str,
                              const char* prefix) noexcept {
  return starts_with_lower(str, std::string_view(prefix));
}

/**
 * Checks if @p str ends with the given @p suffix.
 */
constexpr bool ends_with(std::string_view str,
                         std::string_view suffix) noexcept {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), std::string_view::npos,
                     suffix) == 0;
}

/**
 * Checks if @p str ends with the given @p suffix.
 */
constexpr bool ends_with(std::string_view str, char suffix) noexcept {
  return !str.empty() && std::string_view::traits_type::eq(str.back(), suffix);
}

/**
 * Checks if @p str ends with the given @p suffix.
 */
constexpr bool ends_with(std::string_view str, const char* suffix) noexcept {
  return ends_with(str, std::string_view(suffix));
}

/**
 * Checks if @p str ends with the given @p suffix, ignoring case.
 */
bool ends_with_lower(std::string_view str, std::string_view suffix) noexcept;

/**
 * Checks if @p str ends with the given @p suffix, ignoring case.
 */
constexpr bool ends_with_lower(std::string_view str, char suffix) noexcept {
  return !str.empty() && toLower(str.back()) == toLower(suffix);
}

/**
 * Checks if @p str ends with the given @p suffix, ignoring case.
 */
inline bool ends_with_lower(std::string_view str, const char* suffix) noexcept {
  return ends_with_lower(str, std::string_view(suffix));
}

/**
 * Checks if @p str contains the substring @p other.
 */
constexpr bool contains(std::string_view str, std::string_view other) noexcept {
  return str.find(other) != std::string_view::npos;
}

/**
 * Checks if @p str contains the substring @p other.
 */
constexpr bool contains(std::string_view str, char ch) noexcept {
  return str.find(ch) != std::string_view::npos;
}

/**
 * Checks if @p str contains the substring @p other.
 */
constexpr bool contains(std::string_view str, const char* other) noexcept {
  return str.find(other) != std::string_view::npos;
}

/**
 * Checks if @p str contains the substring @p other, ignoring case.
 */
inline bool contains_lower(std::string_view str,
                           std::string_view other) noexcept {
  return find_lower(str, other) != std::string_view::npos;
}

/**
 * Checks if @p str contains the substring @p other, ignoring case.
 */
inline bool contains_lower(std::string_view str, char ch) noexcept {
  return find_lower(str, ch) != std::string_view::npos;
}

/**
 * Checks if @p str contains the substring @p other, ignoring case.
 */
inline bool contains_lower(std::string_view str, const char* other) noexcept {
  return find_lower(str, other) != std::string_view::npos;
}

/**
 * Return a string_view equal to @p str but with the first @p n elements
 * dropped.
 */
constexpr std::string_view drop_front(
    std::string_view str, std::string_view::size_type n = 1) noexcept {
  str.remove_prefix(n);
  return str;
}

/**
 * Return a string_view equal to @p str but with the last @p n elements dropped.
 */
constexpr std::string_view drop_back(
    std::string_view str, std::string_view::size_type n = 1) noexcept {
  str.remove_suffix(n);
  return str;
}

/**
 * Returns a view equal to @p str but with only the first @p n
 * elements remaining.  If @p n is greater than the length of the
 * string, the entire string is returned.
 */
constexpr std::string_view take_front(
    std::string_view str, std::string_view::size_type n = 1) noexcept {
  auto length = str.size();
  if (n >= length) {
    return str;
  }
  return drop_back(str, length - n);
}

/**
 * Returns a view equal to @p str but with only the last @p n
 * elements remaining.  If @p n is greater than the length of the
 * string, the entire string is returned.
 */
constexpr std::string_view take_back(
    std::string_view str, std::string_view::size_type n = 1) noexcept {
  auto length = str.size();
  if (n >= length) {
    return str;
  }
  return drop_front(str, length - n);
}

/**
 * Returns a reference to the substring of @p str from [start, end).
 *
 * @param start The index of the starting character in the substring; if
 * the index is npos or greater than the length of the string then the
 * empty substring will be returned.
 *
 * @param end The index following the last character to include in the
 * substring. If this is npos or exceeds the number of characters
 * remaining in the string, the string suffix (starting with @p start)
 * will be returned. If this is less than @p start, an empty string will
 * be returned.
 */
constexpr std::string_view slice(std::string_view str,
                                 std::string_view::size_type start,
                                 std::string_view::size_type end) noexcept {
  auto length = str.size();
  start = (std::min)(start, length);
  end = (std::min)((std::max)(start, end), length);
  return {str.data() + start, end - start};
}

/**
 * Splits @p str into two substrings around the first occurrence of a separator
 * character.
 *
 * If @p separator is in the string, then the result is a pair (LHS, RHS)
 * such that (str == LHS + separator + RHS) is true and RHS is
 * maximal. If @p separator is not in the string, then the result is a
 * pair (LHS, RHS) where (str == LHS) and (RHS == "").
 *
 * @param separator The character to split on.
 * @returns The split substrings.
 */
constexpr std::pair<std::string_view, std::string_view> split(
    std::string_view str, char separator) noexcept {
  auto idx = str.find(separator);
  if (idx == std::string_view::npos) {
    return {str, {}};
  }
  return {slice(str, 0, idx), slice(str, idx + 1, std::string_view::npos)};
}

/**
 * Splits @p str into two substrings around the first occurrence of a separator
 * string.
 *
 * If @p separator is in the string, then the result is a pair (LHS, RHS)
 * such that (str == LHS + separator + RHS) is true and RHS is
 * maximal. If @p separator is not in the string, then the result is a
 * pair (LHS, RHS) where (str == LHS) and (RHS == "").
 *
 * @param separator The string to split on.
 * @return The split substrings.
 */
constexpr std::pair<std::string_view, std::string_view> split(
    std::string_view str, std::string_view separator) noexcept {
  auto idx = str.find(separator);
  if (idx == std::string_view::npos) {
    return {str, {}};
  }
  return {slice(str, 0, idx),
          slice(str, idx + separator.size(), std::string_view::npos)};
}

/**
 * Splits @p str into two substrings around the last occurrence of a separator
 * character.
 *
 * If @p separator is in the string, then the result is a pair (LHS, RHS)
 * such that (str == LHS + separator + RHS) is true and RHS is
 * minimal. If @p separator is not in the string, then the result is a
 * pair (LHS, RHS) where (str == LHS) and (RHS == "").
 *
 * @param separator The string to split on.
 * @return The split substrings.
 */
constexpr std::pair<std::string_view, std::string_view> rsplit(
    std::string_view str, char separator) noexcept {
  auto idx = str.rfind(separator);
  if (idx == std::string_view::npos) {
    return {str, {}};
  }
  return {slice(str, 0, idx), slice(str, idx + 1, std::string_view::npos)};
}

/**
 * Splits @p str into two substrings around the last occurrence of a separator
 * string.
 *
 * If @p separator is in the string, then the result is a pair (LHS, RHS)
 * such that (str == LHS + separator + RHS) is true and RHS is
 * minimal. If @p separator is not in the string, then the result is a
 * pair (LHS, RHS) where (str == LHS) and (RHS == "").
 *
 * @param separator The string to split on.
 * @return The split substrings.
 */
constexpr std::pair<std::string_view, std::string_view> rsplit(
    std::string_view str, std::string_view separator) noexcept {
  auto idx = str.rfind(separator);
  if (idx == std::string_view::npos) {
    return {str, {}};
  }
  return {slice(str, 0, idx),
          slice(str, idx + separator.size(), std::string_view::npos)};
}

/**
 * Splits @p str into substrings around the occurrences of a separator string.
 *
 * Each substring is stored in @p arr. If @p maxSplit is >= 0, at most
 * @p maxSplit splits are done and consequently <= @p maxSplit + 1
 * elements are added to arr.
 * If @p keepEmpty is false, empty strings are not added to @p arr. They
 * still count when considering @p maxSplit
 * An useful invariant is that
 * separator.join(arr) == str if maxSplit == -1 and keepEmpty == true
 *
 * @param arr Where to put the substrings.
 * @param separator The string to split on.
 * @param maxSplit The maximum number of times the string is split.
 * @param keepEmpty True if empty substring should be added.
 */
void split(std::string_view str, SmallVectorImpl<std::string_view>& arr,
           std::string_view separator, int maxSplit = -1,
           bool keepEmpty = true) noexcept;

/**
 * Splits @p str into substrings around the occurrences of a separator
 * character.
 *
 * Each substring is stored in @p arr. If @p maxSplit is >= 0, at most
 * @p maxSplit splits are done and consequently <= @p maxSplit + 1
 * elements are added to arr.
 * If @p keepEmpty is false, empty strings are not added to @p arr. They
 * still count when considering @p maxSplit
 * An useful invariant is that
 * separator.join(arr) == str if maxSplit == -1 and keepEmpty == true
 *
 * @param arr Where to put the substrings.
 * @param separator The character to split on.
 * @param maxSplit The maximum number of times the string is split.
 * @param keepEmpty True if empty substring should be added.
 */
void split(std::string_view str, SmallVectorImpl<std::string_view>& arr,
           char separator, int maxSplit = -1, bool keepEmpty = true) noexcept;

/**
 * Returns @p str with consecutive @p ch characters starting from the
 * the left removed.
 */
constexpr std::string_view ltrim(std::string_view str, char ch) noexcept {
  return drop_front(str, (std::min)(str.size(), str.find_first_not_of(ch)));
}

/**
 * Returns @p str with consecutive characters in @p chars starting from
 * the left removed.
 */
constexpr std::string_view ltrim(
    std::string_view str, std::string_view chars = " \t\n\v\f\r") noexcept {
  return drop_front(str, (std::min)(str.size(), str.find_first_not_of(chars)));
}

/**
 * Returns @p str with consecutive @p Char characters starting from the
 * right removed.
 */
constexpr std::string_view rtrim(std::string_view str, char ch) noexcept {
  return drop_back(
      str, str.size() - (std::min)(str.size(), str.find_last_not_of(ch) + 1));
}

/**
 * Returns @p str with consecutive characters in @p chars starting from
 * the right removed.
 */
constexpr std::string_view rtrim(
    std::string_view str, std::string_view chars = " \t\n\v\f\r") noexcept {
  return drop_back(
      str,
      str.size() - (std::min)(str.size(), str.find_last_not_of(chars) + 1));
}

/**
 * Returns @p str with consecutive @p ch characters starting from the
 * left and right removed.
 */
constexpr std::string_view trim(std::string_view str, char ch) noexcept {
  return rtrim(ltrim(str, ch), ch);
}

/**
 * Returns @p str with consecutive characters in @p chars starting from
 * the left and right removed.
 */
constexpr std::string_view trim(
    std::string_view str, std::string_view chars = " \t\n\v\f\r") noexcept {
  return rtrim(ltrim(str, chars), chars);
}

namespace detail {
bool GetAsUnsignedInteger(
    std::string_view str, unsigned radix,
    unsigned long long& result) noexcept;  // NOLINT(runtime/int)
bool GetAsSignedInteger(std::string_view str, unsigned radix,
                        long long& result) noexcept;  // NOLINT(runtime/int)

bool ConsumeUnsignedInteger(
    std::string_view& str, unsigned radix,
    unsigned long long& result) noexcept;  // NOLINT(runtime/int)
bool ConsumeSignedInteger(std::string_view& str, unsigned radix,
                          long long& result) noexcept;  // NOLINT(runtime/int)
}  // namespace detail

/**
 * Parses the string @p str as an integer of the specified radix.  If
 * @p radix is specified as zero, this does radix autosensing using
 * extended C rules: 0 is octal, 0x is hex, 0b is binary.
 *
 * If the string is invalid or if only a subset of the string is valid,
 * this returns nullopt to signify the error.  The string is considered
 * erroneous if empty or if it overflows T.
 */
template <typename T,
          std::enable_if_t<std::numeric_limits<T>::is_signed, bool> = true>
inline std::optional<T> parse_integer(std::string_view str,
                                      unsigned radix) noexcept {
  long long val;  // NOLINT(runtime/int)
  if (detail::GetAsSignedInteger(str, radix, val) ||
      static_cast<T>(val) != val) {
    return std::nullopt;
  }
  return val;
}

template <typename T,
          std::enable_if_t<!std::numeric_limits<T>::is_signed, bool> = true>
inline std::optional<T> parse_integer(std::string_view str,
                                      unsigned radix) noexcept {
  using Int = unsigned long long;  // NOLINT(runtime/int)
  Int val;
  // The additional cast to unsigned long long is required to avoid the
  // Visual C++ warning C4805: '!=' : unsafe mix of type 'bool' and type
  // 'unsigned __int64' when instantiating getAsInteger with T = bool.
  if (detail::GetAsUnsignedInteger(str, radix, val) ||
      static_cast<Int>(static_cast<T>(val)) != val) {
    return std::nullopt;
  }
  return val;
}

/**
 * Parses the string @p str as an integer of the specified radix.  If
 * @p radix is specified as zero, this does radix autosensing using
 * extended C rules: 0 is octal, 0x is hex, 0b is binary.
 *
 * If the string does not begin with a number of the specified radix,
 * this returns nullopt to signify the error. The string is considered
 * erroneous if empty or if it overflows T.
 * The portion of the string representing the discovered numeric value
 * is removed from the beginning of the string.
 */
template <typename T,
          std::enable_if_t<std::numeric_limits<T>::is_signed, bool> = true>
inline std::optional<T> consume_integer(std::string_view* str,
                                        unsigned radix) noexcept {
  using Int = long long;  // NOLINT(runtime/int)
  Int val;
  if (detail::ConsumeSignedInteger(*str, radix, val) ||
      static_cast<Int>(static_cast<T>(val)) != val) {
    return std::nullopt;
  }
  return val;
}

template <typename T,
          std::enable_if_t<!std::numeric_limits<T>::is_signed, bool> = true>
inline std::optional<T> consume_integer(std::string_view* str,
                                        unsigned radix) noexcept {
  using Int = unsigned long long;  // NOLINT(runtime/int)
  Int val;
  if (detail::ConsumeUnsignedInteger(*str, radix, val) ||
      static_cast<Int>(static_cast<T>(val)) != val) {
    return std::nullopt;
  }
  return val;
}

/**
 * Parses the string @p str as a floating point value.
 *
 * If the string is invalid or if only a subset of the string is valid,
 * this returns nullopt to signify the error.  The string is considered
 * erroneous if empty or if it overflows T.
 */
template <typename T>
std::optional<T> parse_float(std::string_view str) noexcept;

template <>
std::optional<float> parse_float<float>(std::string_view str) noexcept;
template <>
std::optional<double> parse_float<double>(std::string_view str) noexcept;
template <>
std::optional<long double> parse_float<long double>(
    std::string_view str) noexcept;

}  // namespace wpi
