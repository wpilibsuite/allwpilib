/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_value.h"

#include <cassert>
#include <cstring>

#include "ntcore.h"

bool
operator== (const NT_Value &lhs, const NT_Value &rhs)
{
    if (lhs.type != rhs.type)
        return 0;
    switch (lhs.type)
    {
    case NT_UNASSIGNED:
        return 1; // XXX: is this better being false instead?
    case NT_BOOLEAN:
        return lhs.data.v_boolean == rhs.data.v_boolean;
    case NT_DOUBLE:
        return lhs.data.v_double == rhs.data.v_double;
    case NT_STRING:
    case NT_RAW:
    case NT_RPC:
        if (lhs.data.v_string.len != rhs.data.v_string.len)
            return 0;
        return std::memcmp(lhs.data.v_string.str, rhs.data.v_string.str,
                           lhs.data.v_string.len) == 0;
    case NT_BOOLEAN_ARRAY:
        if (lhs.data.arr_boolean.size != rhs.data.arr_boolean.size)
            return 0;
        return std::memcmp(lhs.data.arr_boolean.arr, rhs.data.arr_boolean.arr,
                           lhs.data.arr_boolean.size *
                           sizeof(lhs.data.arr_boolean.arr[0])) == 0;
    case NT_DOUBLE_ARRAY:
        if (lhs.data.arr_double.size != rhs.data.arr_double.size)
            return 0;
        return std::memcmp(lhs.data.arr_double.arr, rhs.data.arr_double.arr,
                           lhs.data.arr_double.size *
                           sizeof(lhs.data.arr_double.arr[0])) == 0;
    case NT_STRING_ARRAY:
    {
        if (lhs.data.arr_string.size != rhs.data.arr_string.size)
            return 0;
        for (size_t i=0; i<lhs.data.arr_string.size; i++)
        {
            if (lhs.data.arr_string.arr[i].len !=
                rhs.data.arr_string.arr[i].len)
                return 0;
            if (std::memcmp(lhs.data.arr_string.arr[i].str,
                            rhs.data.arr_string.arr[i].str,
                            lhs.data.arr_string.arr[i].len) != 0)
                return 0;
        }
        return 1;
    }
    default:
        /*assert(0 && "unknown value type");*/
        return 0;
    }
}
