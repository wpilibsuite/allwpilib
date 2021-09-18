// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_VALUE_INTERNAL_H_
#define NTCORE_VALUE_INTERNAL_H_

#include <memory>
#include <string>
#include <string_view>

#include "ntcore_c.h"

namespace nt {

class Value;

void ConvertToC(const Value& in, NT_Value* out);
std::shared_ptr<Value> ConvertFromC(const NT_Value& value);
void ConvertToC(std::string_view in, NT_String* out);
inline std::string_view ConvertFromC(const NT_String& str) {
  return {str.str, str.len};
}

}  // namespace nt

#endif  // NTCORE_VALUE_INTERNAL_H_
