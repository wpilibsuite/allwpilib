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
  UNASSIGNED = NT_UNASSIGNED,
  /// Boolean data type.
  BOOLEAN = NT_BOOLEAN,
  /// Double precision floating-point data type.
  DOUBLE = NT_DOUBLE,
  /// String data type.
  STRING = NT_STRING,
  /// Raw data type.
  RAW = NT_RAW,
  /// Boolean array data type.
  BOOLEAN_ARRAY = NT_BOOLEAN_ARRAY,
  /// Double precision floating-point array data type.
  DOUBLE_ARRAY = NT_DOUBLE_ARRAY,
  /// String array data type.
  STRING_ARRAY = NT_STRING_ARRAY,
  /// Integer data type.
  INTEGER = NT_INTEGER,
  /// Single precision floating-point data type.
  FLOAT = NT_FLOAT,
  /// Integer array data type.
  INTEGER_ARRAY = NT_INTEGER_ARRAY,
  /// Single precision floating-point array data type.
  FLOAT_ARRAY = NT_FLOAT_ARRAY
};

}  // namespace nt
