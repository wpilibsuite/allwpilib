// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "ntcore_c.h"

namespace nt {

std::string_view TypeToString(NT_Type type);
NT_Type StringToType(std::string_view typeStr);
NT_Type StringToType3(std::string_view typeStr);

constexpr bool IsNumeric(NT_Type type) {
  return (type & (NT_INTEGER | NT_FLOAT | NT_DOUBLE)) != 0;
}

constexpr bool IsNumericArray(NT_Type type) {
  return (type & (NT_INTEGER_ARRAY | NT_FLOAT_ARRAY | NT_DOUBLE_ARRAY)) != 0;
}

constexpr bool IsNumericCompatible(NT_Type a, NT_Type b) {
  return (IsNumeric(a) && IsNumeric(b)) ||
         (IsNumericArray(a) && IsNumericArray(b));
}

}  // namespace nt
