/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Value.h"

#include <cassert>
#include <cstring>

#include "ntcore.h"

bool operator==(const NT_Value& lhs, const NT_Value& rhs) {
  if (lhs.type != rhs.type) return false;
  switch (lhs.type) {
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
