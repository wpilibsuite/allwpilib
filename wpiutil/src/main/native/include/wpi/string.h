// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef __cplusplus
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
constexpr std::string_view to_string_view(const WPI_String& str) {
  return {str.str, str.len};
}
}  // namespace wpi
#endif  // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void WPI_InitString(struct WPI_String* wpiString, const char* utf8String);

void WPI_InitStringWithLength(struct WPI_String* wpiString,
                              const char* utf8String, size_t length);

char* WPI_AllocateString(struct WPI_String* wpiString, size_t length);

void WPI_FreeString(const WPI_String* wpiString);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
