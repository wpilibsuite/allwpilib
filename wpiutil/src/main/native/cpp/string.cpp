// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/string.h"

#include <wpi/MemAlloc.h>

#include <string_view>

extern "C" {

void WPI_InitString(struct WPI_String* wpiString, const char* utf8String) {
  if (utf8String == nullptr) {
    wpiString->str = nullptr;
    wpiString->len = 0;
  } else {
    wpiString->str = utf8String;
    wpiString->len = std::char_traits<char>::length(utf8String);
  }
}

void WPI_InitStringWithLength(struct WPI_String* wpiString,
                              const char* utf8String, size_t length) {
  wpiString->str = utf8String;
  wpiString->len = length;
}

char* WPI_AllocateString(struct WPI_String* wpiString, size_t length) {
  char* str = static_cast<char*>(wpi::safe_malloc(length));
  wpiString->str = str;
  wpiString->len = length;
  return str;
}

void WPI_FreeString(const struct WPI_String* wpiString) {
  std::free(const_cast<char*>(wpiString->str));
}

}  // extern "C"
