// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef __cplusplus
#include <cstring>
#include <string_view>
#endif

/**
 * A const UTF8 string.
 */
struct WPI_String {
  /** Contents. */
  const char* str;
  /** Length */
  size_t len;
};

#ifdef __cplusplus
namespace wpi {
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
}  // namespace wpi
#endif  // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * Initializes a WPI_String from a null terminated UTF-8 string.
 * If input string is null, initializes output to 0 length.
 * The output length does not include the null terminator.
 *
 * The lifetime of the output string is the lifetime of the input string.
 * Do not call WPI_FreeString() with the output of this call.
 *
 * @param wpiString output string
 * @param utf8String input string (null terminated)
 */
void WPI_InitString(struct WPI_String* wpiString, const char* utf8String);

/**
 * Initializes a WPI_String from a UTF-8 string and length.
 * If input string is null or 0 length, initializes output to 0 length.
 * The input string does not need to be null terminated.
 *
 * The lifetime of the output string is the lifetime of the input string.
 * Do not call WPI_FreeString() with the output of this call.
 *
 * @param wpiString output string
 * @param utf8String input string
 * @param length input string length in chars
 */
void WPI_InitStringWithLength(struct WPI_String* wpiString,
                              const char* utf8String, size_t length);

/**
 * Allocates a WPI_String for the specified length.
 * The resultant string must be freed with WPI_FreeString().
 *
 * @param wpiString output string
 * @param length string length in chars to allocate
 * @return mutable pointer to allocated buffer
 *
 */
char* WPI_AllocateString(struct WPI_String* wpiString, size_t length);

/**
 * Frees a WPI_String that was allocated with WPI_AllocateString()
 *
 * @param wpiString string to free
 */
void WPI_FreeString(const struct WPI_String* wpiString);

/**
 * Allocates an array of WPI_Strings.
 *
 * @param length array length
 * @return string array
 */
struct WPI_String* WPI_AllocateStringArray(size_t length);

/**
 * Frees a WPI_String array returned by WPI_AllocateStringArray().
 *
 * @param wpiStringArray string array to free
 * @param length length of array
 */
void WPI_FreeStringArray(const struct WPI_String* wpiStringArray,
                         size_t length);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
namespace wpi {

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
}  // namespace wpi
#endif
