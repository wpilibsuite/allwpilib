// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "string"
#include "wpi/string.h"

#include <wpi/MemAlloc.h>

#include <string_view>

extern "C" {

void WPI_InitString(struct WPI_String* wpiString, const char* utf8String) {
  if (wpiString == nullptr) {
    return;
  }
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
  if (wpiString == nullptr) {
    return;
  }
  wpiString->str = utf8String;
  wpiString->len = length;
}

// Returned from AllocateString if 0 length is requested.
// Returned instead of nullptr due to memcpy pointer validity rules
static char writeBuffer;

char* WPI_AllocateString(struct WPI_String* wpiString, size_t length) {
  if (wpiString == nullptr) {
    return nullptr;
  }
  if (length == 0) {
    wpiString->len = 0;
    wpiString->str = nullptr;
    return &writeBuffer;
  }
  char* str = static_cast<char*>(wpi::safe_malloc(length));
  wpiString->str = str;
  wpiString->len = length;
  return str;
}

void WPI_FreeString(const struct WPI_String* wpiString) {
  if (wpiString == nullptr) {
    return;
  }
  std::free(const_cast<char*>(wpiString->str));
}

struct WPI_String* WPI_AllocateStringArray(size_t length) {
  return static_cast<struct WPI_String*>(
      wpi::safe_malloc(length * sizeof(struct WPI_String)));
}

void WPI_FreeStringArray(const struct WPI_String* wpiStringArray,
                         size_t length) {
  if (wpiStringArray == nullptr) {
    return;
  }
  for (size_t i = 0; i < length; ++i) {
    WPI_FreeString(&wpiStringArray[i]);
  }
  std::free(const_cast<struct WPI_String*>(wpiStringArray));
}

}  // extern "C"
