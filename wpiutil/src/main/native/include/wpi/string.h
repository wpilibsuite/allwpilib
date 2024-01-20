// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef __cplusplus
#include <string_view>
#endif

/**
 * A mutable UTF8 string.
 */
struct WPI_MutableString {
#ifdef __cplusplus
  // Implicit conversion to string_view
  operator std::string_view() const { return {str, len}; }
#endif  // __cplusplus

  /** Contents. */
  char* str;
  /** Length */
  size_t len;
};

/**
 * A const UTF8 string.
 */
struct WPI_ConstString {
#ifdef __cplusplus
  constexpr WPI_ConstString(std::string_view view)
      : str{view.data()}, len{view.size()} {}
  constexpr WPI_ConstString(const char* cStr)
      : str{cStr}, len{std::char_traits<char>::length(cStr)} {}

  constexpr WPI_ConstString(WPI_MutableString& mutableStr)
      : str{mutableStr.str}, len{mutableStr.len} {}

  // Implicit conversion to string_view
  operator std::string_view() const { return {str, len}; }
#endif  // __cplusplus

  /** Contents. */
  const char* str;
  /** Length */
  size_t len;
};

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void WPI_InitConstString(WPI_ConstString* wpiString, const char* utf8String);

void WPI_InitConstStringWithLength(WPI_ConstString* wpiString,
                                   const char* utf8String, size_t length);

void WPI_AllocateMutableString(WPI_MutableString* wpiString, size_t length);

void WPI_FreeMutableString(const WPI_MutableString* wpiString);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
