/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Value.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "ntcore.h"

using namespace ntimpl;

StringValue::StringValue(llvm::StringRef val) {
  len = val.size();
  str = static_cast<char*>(std::malloc(len+1));
  std::memcpy(str, val.data(), len);
  str[len] = '\0';
}

void Value::SetBooleanArray(llvm::ArrayRef<int> value) {
  // handle type change
  if (NT_Value::type != NT_BOOLEAN_ARRAY) {
    NT_DisposeValue(this);
    data.arr_boolean.arr = nullptr;
    data.arr_boolean.size = 0;  // set to zero so size change is hit below
    NT_Value::type = NT_BOOLEAN_ARRAY;
  }
  // handle size change
  if (data.arr_boolean.size != value.size()) {
    std::free(data.arr_boolean.arr);
    data.arr_boolean.arr =
        static_cast<int*>(std::malloc(value.size()*sizeof(int)));
    data.arr_boolean.size = value.size();
  }
  std::copy(value.begin(), value.end(), data.arr_boolean.arr);
}

void Value::SetDoubleArray(llvm::ArrayRef<double> value) {
  // handle type change
  if (NT_Value::type != NT_DOUBLE_ARRAY) {
    NT_DisposeValue(this);
    data.arr_double.arr = nullptr;
    data.arr_double.size = 0;  // set to zero so size change is hit below
    NT_Value::type = NT_DOUBLE_ARRAY;
  }
  // handle size change
  if (data.arr_double.size != value.size()) {
    std::free(data.arr_double.arr);
    data.arr_double.arr =
        static_cast<double*>(std::malloc(value.size()*sizeof(double)));
    data.arr_double.size = value.size();
  }
  std::copy(value.begin(), value.end(), data.arr_double.arr);
}

void Value::SetStringArray(std::vector<StringValue>& value) {
  // handle type change
  if (NT_Value::type != NT_STRING_ARRAY) {
    NT_DisposeValue(this);
    data.arr_string.arr = nullptr;
    data.arr_string.size = 0;  // set to zero so size change is hit below
    NT_Value::type = NT_STRING_ARRAY;
  }
  // handle size change
  if (data.arr_string.size != value.size()) {
    std::free(data.arr_string.arr);
    data.arr_string.arr =
        static_cast<NT_String*>(std::malloc(value.size()*sizeof(NT_String)));
    // need to initialize array to avoid invalid frees in std::move below.
    for (size_t i=0; i<value.size(); ++i) {
      data.arr_string.arr[i].str = nullptr;
      data.arr_string.arr[i].len = 0;
    }
    data.arr_string.size = value.size();
  }
  std::move(value.begin(), value.end(),
            static_cast<StringValue*>(data.arr_string.arr));
  value.clear();
}

bool ntimpl::operator==(const Value& lhs, const Value& rhs) {
  if (lhs.type() != rhs.type()) return false;
  switch (lhs.type()) {
    case NT_UNASSIGNED:
      return true;  // XXX: is this better being false instead?
    case NT_BOOLEAN:
      return lhs.data.v_boolean == rhs.data.v_boolean;
    case NT_DOUBLE:
      return lhs.data.v_double == rhs.data.v_double;
    case NT_STRING:
    case NT_RAW:
    case NT_RPC:
      if (lhs.data.v_string.len != rhs.data.v_string.len) return false;
      return std::memcmp(lhs.data.v_string.str, rhs.data.v_string.str,
                         lhs.data.v_string.len) == 0;
    case NT_BOOLEAN_ARRAY:
      if (lhs.data.arr_boolean.size != rhs.data.arr_boolean.size) return false;
      return std::memcmp(lhs.data.arr_boolean.arr, rhs.data.arr_boolean.arr,
                         lhs.data.arr_boolean.size *
                             sizeof(lhs.data.arr_boolean.arr[0])) == 0;
    case NT_DOUBLE_ARRAY:
      if (lhs.data.arr_double.size != rhs.data.arr_double.size) return false;
      return std::memcmp(lhs.data.arr_double.arr, rhs.data.arr_double.arr,
                         lhs.data.arr_double.size *
                             sizeof(lhs.data.arr_double.arr[0])) == 0;
    case NT_STRING_ARRAY: {
      if (lhs.data.arr_string.size != rhs.data.arr_string.size) return false;
      for (size_t i = 0; i < lhs.data.arr_string.size; i++) {
        if (lhs.data.arr_string.arr[i].len != rhs.data.arr_string.arr[i].len)
          return false;
        if (std::memcmp(lhs.data.arr_string.arr[i].str,
                        rhs.data.arr_string.arr[i].str,
                        lhs.data.arr_string.arr[i].len) != 0)
          return false;
      }
      return true;
    }
    default:
      // assert(false && "unknown value type");
      return false;
  }
}
