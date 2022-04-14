// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLETYPE_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLETYPE_H_

#include "ntcore_c.h"

namespace nt {

/**
 * NetworkTable entry type.
 * @ingroup ntcore_cpp_api
 */
enum class NetworkTableType {
  kUnassigned = NT_UNASSIGNED,
  kBoolean = NT_BOOLEAN,
  kDouble = NT_DOUBLE,
  kString = NT_STRING,
  kRaw = NT_RAW,
  kBooleanArray = NT_BOOLEAN_ARRAY,
  kDoubleArray = NT_DOUBLE_ARRAY,
  kStringArray = NT_STRING_ARRAY,
  kRpc = NT_RPC
};

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLETYPE_H_
