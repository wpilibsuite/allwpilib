// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Value_internal.h"

using namespace nt;

Value nt::ConvertNumericValue(const Value& value, NT_Type type) {
  switch (type) {
    case NT_INTEGER: {
      Value newval =
          Value::MakeInteger(GetNumericAs<int64_t>(value), value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    case NT_FLOAT: {
      Value newval = Value::MakeFloat(GetNumericAs<float>(value), value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    case NT_DOUBLE: {
      Value newval =
          Value::MakeDouble(GetNumericAs<double>(value), value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    case NT_INTEGER_ARRAY: {
      Value newval = Value::MakeIntegerArray(GetNumericArrayAs<int64_t>(value),
                                             value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    case NT_FLOAT_ARRAY: {
      Value newval =
          Value::MakeFloatArray(GetNumericArrayAs<float>(value), value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    case NT_DOUBLE_ARRAY: {
      Value newval = Value::MakeDoubleArray(GetNumericArrayAs<double>(value),
                                            value.time());
      newval.SetServerTime(value.server_time());
      return newval;
    }
    default:
      return {};
  }
}
