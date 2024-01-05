// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "ntcore_c.h"

namespace nt {

/**
 * NetworkTable entry type.
 * @ingroup ntcore_cpp_api
 */
enum class NetworkTableType {
  /// Unassigned data type.
  kUnassigned = NT_UNASSIGNED,
  /// Boolean data type.
  kBoolean = NT_BOOLEAN,
  /// Double precision floating-point data type.
  kDouble = NT_DOUBLE,
  /// String data type.
  kString = NT_STRING,
  /// Raw data type.
  kRaw = NT_RAW,
  /// Boolean array data type.
  kBooleanArray = NT_BOOLEAN_ARRAY,
  /// Double precision floating-point array data type.
  kDoubleArray = NT_DOUBLE_ARRAY,
  /// String array data type.
  kStringArray = NT_STRING_ARRAY,
  /// Integer data type.
  kInteger = NT_INTEGER,
  /// Single precision floating-point data type.
  kFloat = NT_FLOAT,
  /// Integer array data type.
  kIntegerArray = NT_INTEGER_ARRAY,
  /// Single precision floating-point array data type.
  kFloatArray = NT_FLOAT_ARRAY
};

}  // namespace nt
