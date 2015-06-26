/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cassert>
#include <cstdlib>

#include "ntcore.h"

void NT_DisposeValue(NT_Value *value) {
  switch (value->type) {
    case NT_UNASSIGNED:
    case NT_BOOLEAN:
    case NT_DOUBLE:
      break;
    case NT_STRING:
    case NT_RAW:
    case NT_RPC:
      std::free(value->data.v_string.str);
      break;
    case NT_BOOLEAN_ARRAY:
      std::free(value->data.arr_boolean.arr);
      break;
    case NT_DOUBLE_ARRAY:
      std::free(value->data.arr_double.arr);
      break;
    case NT_STRING_ARRAY: {
      for (size_t i = 0; i < value->data.arr_string.size; i++)
        std::free(value->data.arr_string.arr[i].str);
      std::free(value->data.arr_string.arr);
      break;
    }
    default:
      assert(0 && "unknown value type");
  }
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
}

void NT_InitValue(NT_Value *value) {
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
}

void NT_DisposeString(NT_String *str) {
  std::free(str->str);
  str->str = nullptr;
  str->len = 0;
}

void NT_InitString(NT_String *str) {
  str->str = nullptr;
  str->len = 0;
}

void NT_DisposeConnectionInfoArray(NT_ConnectionInfo *arr, size_t count) {
  unsigned int i;
  for (i = 0; i < count; i++)
    std::free(arr[i].remote_id.str);
  std::free(arr);
}
