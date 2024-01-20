// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/string.h"

#include <wpi/MemAlloc.h>
#include <string_view>

extern "C" {

void WPI_InitConstString(WPI_ConstString* wpiString, const char* utf8String) {
  wpiString->str = utf8String;
  wpiString->len = std::char_traits<char>::length(utf8String);
}

void WPI_InitConstStringWithLength(WPI_ConstString* wpiString,
                                   const char* utf8String, size_t length) {
  wpiString->str = utf8String;
  wpiString->len = length;
}

void WPI_AllocateMutableString(WPI_MutableString* wpiString, size_t length) {
  wpiString->str = static_cast<char*>(wpi::safe_malloc(length));
  wpiString->len = length;
}

void WPI_FreeMutableString(const WPI_MutableString* wpiString) {
  std::free(wpiString->str);
}

}  // extern "C"
