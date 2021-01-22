// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_VALUE_INTERNAL_H_
#define NTCORE_VALUE_INTERNAL_H_

#include <memory>
#include <string>

#include <wpi/StringRef.h>

#include "ntcore_c.h"

namespace nt {

class Value;

void ConvertToC(const Value& in, NT_Value* out);
std::shared_ptr<Value> ConvertFromC(const NT_Value& value);
void ConvertToC(wpi::StringRef in, NT_String* out);
inline wpi::StringRef ConvertFromC(const NT_String& str) {
  return wpi::StringRef(str.str, str.len);
}

}  // namespace nt

#endif  // NTCORE_VALUE_INTERNAL_H_
