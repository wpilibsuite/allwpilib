/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TYPE_H_
#define NT_TYPE_H_

#include "ntcore_c.h"

namespace nt {

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

#endif  // NT_TYPE_H_
